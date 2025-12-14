import { Connection, LAMPORTS_PER_SOL, PublicKey } from "@solana/web3.js";
import conf from "../src/config.js";

export async function airdrop(publicKey: string, amount: number = 10) {
  const connection = new Connection(conf.solanaRpcEndpoint, "confirmed");
  const pubkey = new PublicKey(publicKey);
  const signature = await connection.requestAirdrop(pubkey, amount * LAMPORTS_PER_SOL);
  const latestBlockhash = await connection.getLatestBlockhash();
  await connection.confirmTransaction({
    signature,
    blockhash: latestBlockhash.blockhash,
    lastValidBlockHeight: latestBlockhash.lastValidBlockHeight,
  });
}
