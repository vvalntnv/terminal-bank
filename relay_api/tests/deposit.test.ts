import request from "supertest";
import { expect } from "chai";
import { __testables__ } from "../src/index.js";
import { Keypair } from "@solana/web3.js";
import { getBase58Decoder } from "@solana/kit";
import { airdrop } from "./utils.js";

const app = __testables__.app;

describe("Deposit Endpoint", () => {
  let privateKey: string;
  let publicKey: string;
  const userAccountId = 1;

  before(async () => {
    const kp = Keypair.generate();
    privateKey = getBase58Decoder().decode(kp.secretKey);
    publicKey = kp.publicKey.toBase58();

    await airdrop(publicKey);

    // Initialize account first
    await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        userAccountId,
        userAccountName: "Deposit Test",
      });
  });

  it("should deposit successfully", async () => {
    const response = await request(app)
      .post("/api/deposit")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        index: userAccountId,
        amount: 1000, // Sending as number
      });

    expect(response.status).to.equal(200);
    expect(response.body).to.have.property("signature");
  });
});
