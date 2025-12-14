import request from "supertest";
import { expect } from "chai";
import { __testables__ } from "../src/index.js";
import { Keypair } from "@solana/web3.js";
import { getBase58Decoder } from "@solana/kit";
import { airdrop } from "./utils.js";

const app = __testables__.app;

describe("Initialize Account Endpoint", () => {
  it("should initialize a user account successfully", async () => {
    const kp = Keypair.generate();
    const privateKey = getBase58Decoder().decode(kp.secretKey);
    const publicKey = kp.publicKey.toBase58();

    await airdrop(publicKey);

    const response = await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        userAccountId: 1,
        userAccountName: "Test Account",
      });

    expect(response.status).to.equal(200);
    expect(response.body).to.have.property("signature");
  });

  it("should fail with invalid wallet headers", async () => {
    const response = await request(app).post("/api/initialize-account").send({
      userAccountId: 1,
      userAccountName: "Test Account",
    });

    expect(response.status).to.equal(403);
  });

  it("should fail with invalid body", async () => {
    const kp = Keypair.generate();
    const privateKey = getBase58Decoder().decode(kp.secretKey);
    const publicKey = kp.publicKey.toBase58();

    const response = await request(app)
      .post("/api/initialize-account")
      .set("x-priv-key", privateKey)
      .set("x-pub-key", publicKey)
      .send({
        userAccountId: -1, // Invalid ID
        userAccountName: "Test Account",
      });

    expect(response.status).to.equal(400);
  });
});
