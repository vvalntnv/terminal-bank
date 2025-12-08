import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Onchain } from "../target/types/onchain";
import {
  createMint,
  getAccount,
  getAssociatedTokenAddress,
} from "@solana/spl-token";
import { expect } from "chai";

describe("External Transfer", () => {
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.onchain as Program<Onchain>;
  const user = provider.wallet as anchor.Wallet;

  let levMint: anchor.web3.PublicKey;
  let mintAuthorityPda: anchor.web3.PublicKey;

  // Main Account (ID = 1) - The fixed receiver
  const mainIndex = 1;
  let mainAccountPda: anchor.web3.PublicKey;
  let mainAta: anchor.web3.PublicKey;

  // Sub Account (ID = 2) - The sender
  const subIndex = 2;
  let subAccountPda: anchor.web3.PublicKey;
  let subAta: anchor.web3.PublicKey;

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

    // 2. Setup Main Account (ID = 1)
    [mainAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([mainIndex]),
      ],
      program.programId
    );
    mainAta = await getAssociatedTokenAddress(levMint, mainAccountPda, true);

    await program.methods
      .initialize(mainIndex, "Main Account")
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda: mainAccountPda,
      })
      .rpc();

    // 3. Setup Sub Account (ID = 2)
    [subAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([subIndex]),
      ],
      program.programId
    );
    subAta = await getAssociatedTokenAddress(levMint, subAccountPda, true);

    await program.methods
      .initialize(subIndex, "Savings Account")
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda: subAccountPda,
      })
      .rpc();

    // 4. Deposit to Sub Account
    // We need to deposit funds into the sub-account so we can transfer them out
    await (program.methods as any)
      .deposit(subIndex, new anchor.BN(1000000)) // 1.000000
      .accounts({
        user: user.publicKey,
        userAccountPda: subAccountPda,
        userAta: subAta,
        levMint,
        mintAuthorityPda,
      })
      .rpc();
  });

  it("Fails to transfer insufficient funds", async () => {
    const amount = new anchor.BN(1100000); // More than 1,000,000

    try {
      await (program.methods as any)
        .externalTransfer(subIndex, user.publicKey, amount)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          senderPdaAccount: subAccountPda,
          senderAta: subAta,
          receiverPdaAccount: mainAccountPda,
          receiverAtaAccount: mainAta,
        })
        .rpc();
      expect.fail("Should have failed");
    } catch (e: any) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal("InsufficientFunds");
      } else {
        expect.fail(`Wrong error type: ${e}`);
      }
    }
  });

  it("Fails when trying to transfer to self", async () => {
    const amount = new anchor.BN(100000);

    try {
      await (program.methods as any)
        .externalTransfer(subIndex, user.publicKey, amount)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          senderPdaAccount: subAccountPda,
          senderAta: subAta,
          receiverPdaAccount: mainAccountPda,
          receiverAtaAccount: mainAta,
        })
        .rpc();

      expect.fail("Should have failed due to self-transfer check");
    } catch (e: any) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal(
          "CannotTransferTokensBetweenSameAccounts"
        );
      } else {
        expect.fail(`Wrong error type: ${e}`);
      }
    }
  });

  it("Transfers tokens from User 1 Sub Account to User 2 Main Account", async () => {
    // 1. Create User 2
    const secondUser = anchor.web3.Keypair.generate();

    // 2. Airdrop SOL to User 2 (needed to initialize their account)
    const signature = await provider.connection.requestAirdrop(
      secondUser.publicKey,
      1 * anchor.web3.LAMPORTS_PER_SOL
    );
    await provider.connection.confirmTransaction(signature);

    // 3. Derive User 2 Main Account PDA (ID = 1)
    const [secondUserMainPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        secondUser.publicKey.toBuffer(),
        Buffer.from([1]), // Main Index
      ],
      program.programId
    );
    const secondUserMainAta = await getAssociatedTokenAddress(
      levMint,
      secondUserMainPda,
      true
    );

    // 4. Initialize User 2 Main Account
    await program.methods
      .initialize(1, "User 2 Main")
      .accounts({
        user: secondUser.publicKey,
        levMint: levMint,
        userAccountPda: secondUserMainPda,
      })
      .signers([secondUser])
      .rpc();

    // 5. Transfer from User 1 Sub (subIndex=2) to User 2 Main
    // User 1 is the signer and sender. User 2 is the receiver.
    const amount = new anchor.BN(100000);

    await (program.methods as any)
      .externalTransfer(subIndex, secondUser.publicKey, amount)
      .accounts({
        user: user.publicKey, // User 1 is signer
        levMint: levMint,
        senderPdaAccount: subAccountPda, // User 1 Sub
        senderAta: subAta,
        receiverPdaAccount: secondUserMainPda, // User 2 Main
        receiverAtaAccount: secondUserMainAta,
      })
      .rpc();

    // 6. Verify Balances (if it succeeded)
    const subAccount = await getAccount(provider.connection, subAta);
    const mainAccount = await getAccount(
      provider.connection,
      secondUserMainAta
    );

    // Sub: 1,000,000 (initial) - 100,000 = 900,000
    expect(subAccount.amount.toString()).to.equal("900000");
    // Main (User 2): 0 + 100,000 = 100,000
    expect(mainAccount.amount.toString()).to.equal("100000");
  });
});
