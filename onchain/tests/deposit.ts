import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Onchain } from "../target/types/onchain";
import {
  createMint,
  getAccount,
  getAssociatedTokenAddress,
} from "@solana/spl-token";
import { expect } from "chai";

describe("Deposit", () => {
  // Configure the client to use the local cluster.
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.onchain as Program<Onchain>;
  const user = provider.wallet as anchor.Wallet;

  let levMint: anchor.web3.PublicKey;
  let mintAuthorityPda: anchor.web3.PublicKey;
  let userAccountPda: anchor.web3.PublicKey;
  let userAta: anchor.web3.PublicKey;

  // Use a different index to avoid collisions with onchain.ts if running together
  const index = 99;
  const accountName = "Deposit Account";

  before(async () => {
    // 1. Derive Mint Authority PDA
    // This must match the seeds in the program: MINT_AUTH_SEED = b"mint-auth"
    [mintAuthorityPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [Buffer.from("mint-auth")],
      program.programId
    );

    // 2. Create a mint for testing (LEV mint)
    // IMPORTANT: The authority must be the mintAuthorityPda so the program can mint tokens
    levMint = await createMint(
      provider.connection,
      user.payer,
      mintAuthorityPda, // Mint Authority
      null, // Freeze Authority
      6 // Decimals
    );

    // 3. Derive User Account PDA
    [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    // 4. Derive User ATA (Destination for deposit)
    userAta = await getAssociatedTokenAddress(
      levMint,
      userAccountPda,
      true // allowOwnerOffCurve = true because owner is a PDA
    );

    // 5. Initialize the User Subaccount (prerequisite)
    try {
      await program.methods
        .initialize(index, accountName)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          userAccountPda,
        })
        .rpc();
    } catch (e) {
      console.error("Initialization failed:", e);
      throw e;
    }
  });

  it("Deposits successfully", async () => {
    const amount = new anchor.BN(1000000); // 1.000000 token

    await (program.methods as any)
      .deposit(index, amount)
      .accounts({
        user: user.publicKey,
        userAccountPda,
        userAta,
        levMint,
        mintAuthorityPda,
      })
      .rpc();

    // Verify token balance
    const userAtaAccount = await getAccount(provider.connection, userAta);
    expect(userAtaAccount.amount.toString()).to.equal(amount.toString());
  });

  it("Deposits additional amount successfully", async () => {
    const amount = new anchor.BN(500000); // 0.500000 token

    await (program.methods as any)
      .deposit(index, amount)
      .accounts({
        user: user.publicKey,
        userAccountPda,
        // userAta,
        levMint,
        // mintAuthorityPda,
      })
      .rpc();

    // Verify cumulative balance (1,000,000 + 500,000 = 1,500,000)
    const userAtaAccount = await getAccount(provider.connection, userAta);
    expect(userAtaAccount.amount.toString()).to.equal("1500000");
  });

  it("Fails to deposit with invalid index (uninitialized account)", async () => {
    const invalidIndex = 100;
    const amount = new anchor.BN(1000);

    const [invalidUserAccountPda] =
      anchor.web3.PublicKey.findProgramAddressSync(
        [
          Buffer.from("user-account"),
          user.publicKey.toBuffer(),
          Buffer.from([invalidIndex]),
        ],
        program.programId
      );

    try {
      await (program.methods as any)
        .deposit(invalidIndex, amount)
        .accounts({
          user: user.publicKey,
          userAccountPda: invalidUserAccountPda,
          // userAta: invalidUserAta, // Note: This ATA probably doesn't exist yet either
          levMint,
          // mintAuthorityPda,
        })
        .rpc();
      expect.fail(
        "Should have failed due to AccountNotInitialized or similar constraint"
      );
    } catch (e) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal("AccountNotInitialized");
        expect(e.error.errorCode.number).to.equal(3012);

        return;
      }

      expect.fail(`A wrong error type has occured: ${e}`);
    }
  });
});
