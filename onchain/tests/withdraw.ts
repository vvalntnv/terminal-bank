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
  const index = 10;
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
      await (program.methods as any)
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
    const sig = await (program.methods as any)
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
    const userAtaData = await getAccount(provider.connection, userAta);
    const balance = userAtaData.amount;

    // 2. Call withdraw with the exact current balance
    await (program.methods as any)
      .withdraw(index, new anchor.BN(balance.toString()))
      .accounts({
        user: user.publicKey,
        userAccountPda: userAccountPda,
        levMint,
      })
      .signers([user.payer])
      .rpc();

    // 3. Assert that the new token balance is 0
    const userAtaDataAfter = await getAccount(provider.connection, userAta);
    expect(userAtaDataAfter.amount.toString()).to.equal("0");
  });

  it("Fail to withdraw more than available balance", async () => {
    // 1. Try to withdraw an amount larger than the account balance
    const amount = new anchor.BN(1);

    try {
      await (program.methods as any)
        .withdraw(index, amount)
        .accounts({
          user: user.publicKey,
          userAccountPda: userAccountPda,
          levMint,
        })
        .signers([user.payer])
        .rpc();
      expect.fail("Should have failed due to Insufficient Funds");
    } catch (e) {
      if (e instanceof anchor.AnchorError) {
        expect(e.error.errorCode.code).to.equal("InsufficientFunds");
        return;
      }
      expect.fail(`A wrong error type has occured: ${e}`);
    }
  });

  it("Fail to withdraw from valid account with wrong signer", async () => {
    // 1. Create a dummy keypair
    const dummy = anchor.web3.Keypair.generate();
    // Airdrop some SOL to dummy so it can pay for transaction fees if needed (though user.payer pays usually, here dummy is signer so dummy pays)
    // Wait, if dummy is signer, dummy must pay fees unless we have another payer.
    // In .signers([dummy]), dummy is the signer.
    // .accounts({ user: dummy.publicKey, ... }) -> user is mut Signer.
    // So dummy must pay. We need to fund dummy.

    const signature = await provider.connection.requestAirdrop(
      dummy.publicKey,
      1 * anchor.web3.LAMPORTS_PER_SOL
    );
    await provider.connection.confirmTransaction(signature);

    const amount = new anchor.BN(10);

    // 2. Attempt to call withdraw using the user's index/accounts but signing with the dummy keypair
    try {
      await (program.methods as any)
        .withdraw(index, amount)
        .accounts({
          user: dummy.publicKey,
          userAccountPda: userAccountPda, // This PDA belongs to original user
          levMint,
        })
        .signers([dummy])
        .rpc();
      expect.fail("Should have failed due to constraint seeds");
    } catch (e) {
      // 3. Expect an error (Signer constraint mismatch or signature verification failure)
      if (e instanceof anchor.AnchorError) {
        // ConstraintSeeds because userAccountPda seeds won't match dummy key
        expect(e.error.errorCode.code).to.equal("ConstraintSeeds");
        return;
      }
      // Sometimes it might be "ConstraintHasOne" if checking fields, but here seeds.
      // Or "AccountNotInitialized" if it tries to derive and fails?
      // No, we passed an account that exists (userAccountPda).
      // But we said it should be derived from [seed, dummy, index].
      // Anchor will check if passed account address == derived address.
      // They won't match. So ConstraintSeeds.
      expect.fail(`A wrong error type has occured: ${e}`);
    }
  });
});
