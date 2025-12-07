import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Onchain } from "../target/types/onchain";
import {
  createMint,
  getAccount,
  getAssociatedTokenAddress,
  TOKEN_PROGRAM_ID,
} from "@solana/spl-token";
import { expect } from "chai";

describe("withdraw", () => {
  // Configure the client to use the local cluster.
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.onchain as Program<Onchain>;

  // Test state variables
  const user = provider.wallet as anchor.Wallet;
  let levMint: anchor.web3.PublicKey;
  let mintAuthorityPda: anchor.web3.PublicKey;
  let userAccountPda: anchor.web3.PublicKey;
  let userAta: anchor.web3.PublicKey;

  // Constants
  const index = 0;
  const accountName = "Withdraw Account";
  const initialDeposit = new anchor.BN(50_000_000); // 50 LEV

  before(async () => {
    // 1. Derive Mint Authority PDA
    // This is needed because the 'deposit' instruction uses it to mint tokens
    [mintAuthorityPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [Buffer.from("mint-auth")],
      program.programId
    );

    // 2. Create the LEV Mint
    // We set the mint authority to the PDA so the program can mint tokens
    levMint = await createMint(
      provider.connection,
      user.payer,
      mintAuthorityPda, // authority
      null, // freeze authority
      6 // decimals
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

    // 4. Derive User ATA
    userAta = await getAssociatedTokenAddress(
      levMint,
      userAccountPda,
      true // allowOwnerOffCurve = true (PDA owner)
    );

    // 5. Initialize the User Subaccount
    // This creates the UserAccount PDA and the User ATA
    try {
      await program.methods
        .initialize(index, accountName)
        .accounts({
          user: user.publicKey,
          userAccountPda,
          levMint: levMint,
        })
        .rpc();
    } catch (e) {
      console.error("Initialize failed:", e);
      throw e;
    }

    // 6. Deposit tokens to the account
    // This ensures there are tokens to withdraw in the tests
    try {
      // Cast to any because the types might be outdated in the local environment
      await (program.methods as any)
        .deposit(index, initialDeposit)
        .accounts({
          user: user.publicKey,
          userAccountPda: userAccountPda,
          levMint: levMint,
        })
        .rpc();
    } catch (e) {
      console.error("Deposit failed:", e);
      throw e;
    }

    // Verify setup
    const balance = await provider.connection.getTokenAccountBalance(userAta);
    expect(balance.value.amount).to.equal(initialDeposit.toString());
  });

  it("Setup successful", async () => {
    // This test confirms that the before hook ran successfully
    // and the environment is ready for withdraw tests.
    const account = await program.account.userAccount.fetch(userAccountPda);
    expect(account.name).to.equal(accountName);
  });

  it("Withdraw partial amount successfully", async () => {
    const amountWithdrawn = new anchor.BN(100);
    const sig = await program.methods
      .withdraw(index, amountWithdrawn)
      .accounts({
        user: user.publicKey,
        userAccountPda: userAccountPda,
        levMint,
      })
      .signers([user.payer])
      .rpc();

    const userAtaData = await getAccount(provider.connection, userAta);
    console.log("user balance", userAtaData.amount);
    expect(userAtaData.amount.toString()).to.equal(
      String(initialDeposit.toNumber() - amountWithdrawn.toNumber())
    );
  });

  it("Withdraw full remaining amount successfully", async () => {
    // 1. Fetch current balance
    // 2. Call withdraw with the exact current balance
    // 3. Assert that the new token balance is 0
  });

  it("Fail to withdraw more than available balance", async () => {
    // 1. Try to withdraw an amount larger than the account balance
    // 2. Expect the transaction to fail (likely with an Insufficient Funds error)
  });

  it("Fail to withdraw from valid account with wrong signer", async () => {
    // 1. Create a dummy keypair
    // 2. Attempt to call withdraw using the user's index/accounts but signing with the dummy keypair
    // 3. Expect an error (Signer constraint mismatch or signature verification failure)
  });
});
