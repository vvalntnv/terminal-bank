import {
  getAddressEncoder,
  getProgramDerivedAddress,
  getPublicKeyFromAddress,
  getU8Encoder,
  getUtf8Encoder,
  type Address,
  type ProgramDerivedAddress,
} from "@solana/kit";
import { ONCHAIN_PROGRAM_ADDRESS } from "../instructionClient/programs/onchain";
import { getMintAddress } from "./mintUtils";
import { getAssociatedTokenAddress } from "@solana/spl-token";

const USER_ACCOUNT_SEED = "user-account";
/**
 * Gets the user account PDA.
 * The address is obtained via the following combination
 * that form a seed:
 * - USER_ACCOUNT_SEED = 'user-account'
 * - userAddress
 * - index of the userAccount (this is the arg of the function
 */
export async function getUserAccountPda(
  userAddress: Address,
  index: number,
): Promise<ProgramDerivedAddress> {
  return await getProgramDerivedAddress({
    programAddress: ONCHAIN_PROGRAM_ADDRESS,
    seeds: [
      getUtf8Encoder().encode(USER_ACCOUNT_SEED),
      getAddressEncoder().encode(userAddress),
      getU8Encoder().encode(index),
    ],
  });
}

/**
 * Gets a user ATA using the User PDA.
 * It is derived using the user's pda and the levMint
 * The lev mint can be obtained using the getMintAddress function
 */
export async function getUserAta(userAccountPda: Address) {
  const levMint = getMintAddress();
  const mintPubkey = await getPublicKeyFromAddress(levMint);
  const userPdaPubkey = await getPublicKeyFromAddress(userAccountPda);

  return await getAssociatedTokenAddress(mintPubkey, userPdaPubkey, true);
}
