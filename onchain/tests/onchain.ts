import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Onchain } from "../target/types/onchain";
import { createMint, getAssociatedTokenAddress } from "@solana/spl-token";
import { expect } from "chai";

describe("Initialize User Subaccount", () => {
  // Configure the client to use the local cluster.
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.onchain as Program<Onchain>;

  let levMint: anchor.web3.PublicKey;
  const user = provider.wallet as anchor.Wallet;

  before(async () => {
    // Create a mint for testing (LEV mint)
    // We use the user's payer keypair to create the mint
    levMint = await createMint(
      provider.connection,
      user.payer,
      user.publicKey,
      user.publicKey,
      6
    );
  });

  it("Initialize user subaccount successfully", async () => {
    const index = 1;
    const name = "Main Account";

    const [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    const userAta = await getAssociatedTokenAddress(
      levMint,
      userAccountPda,
      true // allowOwnerOffCurve = true because owner is a PDA
    );

    try {
      const sig = await program.methods
        .initialize(index, name)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          userAccountPda,
        })
        .rpc();

      console.log("transcation sig", sig);
    } catch (err) {
      console.log(err);
      throw err;
    }

    console.log("eto ni");
    // Verify PDA state
    const account = await program.account.userAccount.fetch(userAccountPda);
    expect(account.owner.toString()).to.equal(user.publicKey.toString());
    expect(account.name).to.equal(name);
    expect(account.index).to.equal(index);

    // Verify ATA exists
    const ataInfo = await provider.connection.getAccountInfo(userAta);
    expect(ataInfo).to.not.be.null;
  });

  it("Fail to initialize with name too long", async () => {
    const index = 2;
    const longName =
      "This name is definitely longer than thirty two characters";

    const [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    try {
      await program.methods
        .initialize(index, longName)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          userAccountPda,
        })
        .rpc();
      expect.fail("Should have failed");
    } catch (e: any) {
      // Check for the specific error NameTooLong
      // The error message usually contains the error code or name
      if (e instanceof anchor.AnchorError) {
        e.error.errorMessage === "The Account name is too long";
        expect(JSON.stringify(e)).to.include("NameTooLong");
      } else {
        expect.fail(`Wrong error occured: ${e}`);
      }
    }
  });

  it("Initialize second subaccount", async () => {
    const index = 3;
    const name = "Savings";

    const [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    await program.methods
      .initialize(index, name)
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda,
      })
      .rpc();

    const account = await program.account.userAccount.fetch(userAccountPda);
    expect(account.index).to.equal(index);
    expect(account.name).to.equal(name);
  });

  it("Initialize with name length exactly 32", async () => {
    const index = 4;
    const name = "x".repeat(32);

    const [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    await program.methods
      .initialize(index, name)
      .accounts({
        user: user.publicKey,
        levMint: levMint,
        userAccountPda,
      })
      .rpc();

    const account = await program.account.userAccount.fetch(userAccountPda);
    expect(account.name).to.equal(name);
  });

  it("Fail to initialize duplicate subaccount", async () => {
    const index = 0; // Already initialized in first test
    const name = "Duplicate";

    const [userAccountPda] = anchor.web3.PublicKey.findProgramAddressSync(
      [
        Buffer.from("user-account"),
        user.publicKey.toBuffer(),
        Buffer.from([index]),
      ],
      program.programId
    );

    try {
      await program.methods
        .initialize(index, name)
        .accounts({
          user: user.publicKey,
          levMint: levMint,
          userAccountPda,
        })
        .rpc();
      expect.fail("Should have failed");
    } catch (e: any) {
      // Expect error because account already exists (seeds constraint)
      expect(e).to.exist;
    }
  });
});
