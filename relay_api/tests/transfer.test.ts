import request from "supertest";
import { expect } from "chai";
import { app } from "../src/index.js";
import { Keypair } from "@solana/web3.js";
import { getBase58Decoder } from "@solana/kit";
import { airdrop } from "./utils.js";

describe("Transfer Endpoints", () => {
  let privateKey: string;
  let publicKey: string;
  const senderIndex = 1;
  const receiverIndex = 2;

  before(async () => {
    const kp = Keypair.generate();
    privateKey = getBase58Decoder().decode(kp.secretKey);
    publicKey = kp.publicKey.toBase58();

    await airdrop(publicKey);

    // Initialize sender account
    await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        userAccountId: senderIndex,
        userAccountName: "Sender",
      });

    // Initialize receiver account
    await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        userAccountId: receiverIndex,
        userAccountName: "Receiver",
      });

    // Deposit to sender account
    await request(app)
      .post("/api/deposit")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        index: senderIndex,
        amount: 1000,
      });
  });

  it("should perform internal transfer successfully", async () => {
    const response = await request(app)
      .post("/api/transfer/internal")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        senderIndex,
        receiverIndex,
        amount: 100,
      });

    expect(response.status).to.equal(200);
    expect(response.body).to.have.property("signature");
  });

  it("should perform external transfer successfully", async () => {
    const recipientKp = Keypair.generate();
    const recipientPrivKey = getBase58Decoder().decode(recipientKp.secretKey);
    const recipientPubKey = recipientKp.publicKey.toBase58();

    await airdrop(recipientPubKey);

    // Initialize recipient account (index 1)
    await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", recipientPrivKey)
      .set("x-pub-key", recipientPubKey)
      .send({
        userAccountId: 1,
        userAccountName: "External Recipient",
      });

    const response = await request(app)
      .post("/api/transfer/external")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        senderIndex,
        to: recipientPubKey,
        amount: 100,
      });

    expect(response.status).to.equal(200);
    expect(response.body).to.have.property("signature");
  });
});
