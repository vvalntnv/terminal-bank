import { assertIsAddress, type Address } from "@solana/kit";
import conf from "../config";

let isMintAddressConfirmed = false;
let isMintAuthAddressConfirmed = false;

/**
 * gets the mint address from the Config
 */
export function getMintAddress(): Address {
  if (!isMintAddressConfirmed) {
    assertIsAddress(conf.mintAddress);
    isMintAddressConfirmed = true;
  }
  return conf.mintAddress as Address;
}

/**
 * gets the mint auth address from the Config
 */
export function getMintAuthAddress(): Address {
  if (!isMintAddressConfirmed) {
    assertIsAddress(conf.mintAuthAddress);
    isMintAuthAddressConfirmed = true;
  }
  return conf.mintAuthAddress as Address;
}
