import * as anchor from "@coral-xyz/anchor";
import { createMint, getMint } from "@solana/spl-token";
import { Keypair, PublicKey } from "@solana/web3.js";
import { Onchain } from "../target/types/onchain";

export async function initMint(provider: anchor.AnchorProvider) {
  const program = anchor.workspace.Onchain as anchor.Program<Onchain>;
  const PROGRAM_ID = new PublicKey(program.programId);

  const SEED_PREFIX = "mint-auth";
  const [mintAuthorityPda, mintAuthorityBump] =
    PublicKey.findProgramAddressSync([Buffer.from(SEED_PREFIX)], PROGRAM_ID);

  console.log("--- Setup ---");
  console.log("Program ID:", PROGRAM_ID.toString());
  console.log("Mint Authority PDA:", mintAuthorityPda.toString());
  console.log("PDA Bump:", mintAuthorityBump);

  const mintKeypair = Keypair.generate();
  console.log("New Mint Address:", mintKeypair.publicKey.toString());

  console.log("\n--- Creating Mint ---");

  try {
    const mintAddress = await createMint(
      provider.connection, // Connection
      provider.wallet.payer, // Payer (the deployer wallet)
      mintAuthorityPda, // Mint Authority (Our PDA) <--- CRITICAL STEP
      mintAuthorityPda, // Freeze Authority (Optional, usually same as Mint Auth)
      9, // Decimals (Standard SOL is 9)
      mintKeypair // The mint keypair (signer)
    );

    console.log("✅ Mint created successfully!");

    // 6. Verify the Authority
    const mintInfo = await getMint(provider.connection, mintAddress);

    console.log("\n--- Verification ---");
    console.log(
      "Actual Mint Authority on-chain:",
      mintInfo.mintAuthority?.toString()
    );
    console.log("Expected PDA:", mintAuthorityPda.toString());

    if (mintInfo.mintAuthority?.toString() === mintAuthorityPda.toString()) {
      console.log("SUCCESS: The PDA is strictly the mint authority.");
    } else {
      console.log("ERROR: Authority mismatch.");
    }
  } catch (error) {
    console.error("Failed to create mint:", error);
  }
}
