import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Onchain } from "../target/types/onchain";
import {
  createMint,
  getAccount,
  getAssociatedTokenAddress,
} from "@solana/spl-token";
import { expect } from "chai";

describe("Internal Transfer", () => {
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.onchain as Program<Onchain>;
  const user = provider.wallet as anchor.Wallet;

  let levMint: anchor.web3.PublicKey;
  let mintAuthorityPda: anchor.web3.PublicKey;

  // Account A
  const indexA = 10;
  let userAccountPdaA: anchor.web3.PublicKey;
  let userAtaA: anchor.web3.PublicKey;

  // Account B
  const indexB = 11;
  let userAccountPdaB: anchor.web3.PublicKey;
  let userAtaB: anchor.web3.PublicKey;

  before(async () => {
    // 1. Setup Mint
    [mintAuthorityPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [Buffer.from("mint-auth")],
      program.programId
    );

    levMint = await createMint(
      provider.connection,
      user.payer,
      mintAuthorityPda,
      null,
      6
    );

    // 2. Setup Account A
    [userAccountPdaA] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([indexA]),
      ],
      program.programId
    );
    userAtaA = await getAssociatedTokenAddress(levMint, userAccountPdaA, true);

    await program.methods
      .initialize(indexA, "Account A")
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda: userAccountPdaA,
      })
      .rpc();

    // 3. Setup Account B
    [userAccountPdaB] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([indexB]),
      ],
      program.programId
    );
    userAtaB = await getAssociatedTokenAddress(levMint, userAccountPdaB, true);

    await program.methods
      .initialize(indexB, "Account B")
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda: userAccountPdaB,
      })
      .rpc();

    // 4. Deposit to Account A
    await (program.methods as any)
      .deposit(indexA, new anchor.BN(1000))
      .accounts({
        user: user.publicKey,
        userAccountPda: userAccountPdaA,
        userAta: userAtaA,
        levMint,
        mintAuthorityPda,
      })
      .rpc();
  });

  it("Transfers tokens from A to B successfully", async () => {
    const amount = new anchor.BN(400);

    await (program.methods as any)
      .internalTransfer(indexA, indexB, amount)
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        senderAccountPda: userAccountPdaA,
        receiverAccountPda: userAccountPdaB,
      })
      .rpc();

    const accountA = await getAccount(provider.connection, userAtaA);
    const accountB = await getAccount(provider.connection, userAtaB);

    expect(accountA.amount.toString()).to.equal("600");
    expect(accountB.amount.toString()).to.equal("400");
  });

  it("Fails to transfer insufficient funds", async () => {
    const amount = new anchor.BN(700); // Only 600 left

    try {
      await (program.methods as any)
        .internalTransfer(indexA, indexB, amount)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          senderAccountPda: userAccountPdaA,
          receiverAccountPda: userAccountPdaB,
        })
        .rpc();
      expect.fail("Should have failed");
    } catch (e: unknown) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal("InsufficientFunds");
        return;
      }
      expect.fail(`received different error type: ${e}`);
    }
  });

  it("Transfer to self (Aliasing check)", async () => {
    const amount = new anchor.BN(100);
    // Transfer from A to A
    try {
      await (program.methods as any)
        .internalTransfer(indexA, indexA, amount)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          senderAccountPda: userAccountPdaA,
          senderAta: userAtaA,
          receiverAccountPda: userAccountPdaA, // Same account
          receiverAta: userAtaA, // Same account
        })
        .rpc();

      // If it succeeds, check balance remains same
      const accountA = await getAccount(provider.connection, userAtaA);
      expect(accountA.amount.toString()).to.equal("600");
    } catch (e: any) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal(
          "CannotTransferTokensBetweenSameAccounts"
        );
        return;
      }

      expect.fail(`received different error type: ${e}`);
    }
  });
});
