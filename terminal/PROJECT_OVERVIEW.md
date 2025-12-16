# **Bank Terminal App — Project Overview (For Coding Agent Context)**

## **1. Purpose**

A decentralized “banking” prototype that simulates a traditional bank experience using the Solana blockchain. Users interact through a **C++ terminal UI**, while the system executes real token operations on-chain. The goal is full transparency: balances and transfers come directly from blockchain state, not from a server-maintained ledger.

---

## **2. Core Components**

### **A. On-Chain Program (Solana / Anchor)**

* Defines the rules for account creation, token minting, burning, transfers, and authority management.
* Uses **PDAs** to represent the bank’s internal structures such as:

  * Mint authority
  * Freeze authority
  * Per-user PDAs (multiple per wallet)
* Operates with a **custom token** (LEV) that behaves as the internal "bank currency".

### **B. API Layer (TypeScript + Express)**

* Serves as a stateless bridge between the TUI and Solana RPC.
* Handles transaction creation, signing (if needed), and submission.
* Offers simplified endpoints for:

  * Account creation flow
  * Deposits (mint)
  * Withdrawals (burn)
  * Transfers
  * Balance lookup

**Important:**
The API holds **no internal state**. All persistence is on chain or in the local TUI SQLite database.
(As noted in the diagram: *“TS API is STATELESS!!!”*)

### **C. Terminal UI (C++ TUI)**

* Local interface for users to operate the “bank.”
* Handles onboarding, keypair generation, terminal interaction, and session state.
* Stores **only user metadata + public keys** in a small local SQLite database.
* Private keys never leave memory.

---

## **3. Local Database (SQLite)**

The C++ TUI maintains a minimal internal database containing:

### **User Table**

### **Bank Accounts Table**

### **Events Table (simple indexer)**
---

## **4. User Account Model**

A single wallet may own multiple PDAs, each representing different “bank accounts,” like:

* Savings
* Food money
* General account

Each PDA has its own ATA for LEV tokens. The Solana program derives these addresses deterministically.

---

## **5. System Flows**

### **A. Create Bank Account (Onboarding)**

1. User opens terminal.
2. User enters personal details.
3. System generates a local private/public keypair.
4. Private key is shown once and kept only in memory.
5. Public key is stored in SQLite.
6. A new on-chain PDA structure is created for the user's bank account.

---

### **B. Deposit Money**

1. User inserts private key into session.
2. User enters deposit amount.
3. System mints the corresponding amount of LEV tokens.
4. Tokens are transferred into the user's ATA.

---

### **C. Transfer Between Bank Accounts**

1. User selects sender and recipient public keys.
2. The program performs a token transfer on-chain.
3. No database changes occur.
4. Resulting balance is fetched from chain.

---

### **D. Withdraw Funds**

1. User enters withdrawal amount.
2. System checks if the user has enough LEV.
3. If yes:

   * Tokens are burned.
   * User receives “BGN” (conceptual fiat) in the simulation.
4. If no: flow terminates with insufficient funds.

---

IMPORTANT, the above are not the full flows, I expect them to change a lot during our coding sessions

## **6. Architecture Philosophy**

* **Transparency:**
  All balances and operations come from real token state on Solana.

* **Decentralization:**
  No centralized backend ledger; all value flows via PDAs + ATAs.

* **Modularity:**

  * Solana program handles rule enforcement.
  * TypeScript API handles communication.
  * C++ TUI handles UX + local persistence.

* **Extendability:**
  Architecture supports future additions like transaction history indexers or multi-token support.

---

## **8. High-Level Summary for Coding Agent**

> *A decentralized banking simulation built on Solana. Users interact through a local C++ terminal that stores minimal user metadata and derives PDAs for multiple bank accounts per user. The system uses LEV, a custom SPL-Token-2022 mint. A Rust/Anchor program enforces all minting, burning, freezing, and transfers. A stateless TypeScript API facilitates RPC access while keeping all balances and operations fully on-chain. SQLite is used only for local metadata, not for financial state.*


---

# Raw Techincal Details

## Terminal Application

### Overview
The terminal application will be written entierly in C++ and should implement a user-friendly TUI, just like Opencode does.
The terminal communication will talk with the Relay API, that is just a communication wrapper around the Solana Program

### Modules configuration
The modules configuration will be as follows (it keeps DDD as its main philosophy):
- infra - The module that does the communication with the external infrastructure
    - database - general queries that the TerminalBank will use. It should be protected from SQL injection
    - relay - API client that will talk to the Relay API
- services - The functional module of the API 
    - RelayService - The RelayService class and its helper classes. Here are the main Relay functionalities are implemented
    - DatabaseService - The DatabaseService class and its helper classes. Here are defined the functions that will READ/WRITE from the SQLite database
- tui - The User-facing part of the application. This is what the user sees
    - components - Reusable TUI components.
    - screens - Different screens that the user can enter
        - CreateAccountScreen - Where the user is onboarded for the first time
        - CreateBankAccountScreen - Where the user creates bank accounts (the application can have MANY bank accounts tied to one wallet /i.e. ATAs that are derived using the id of the bank account in the database)
        - DepositAndWithdrawScreen - Where the user will deposit/withdraw their money. The user should chose from a list with the bank account names
        - ExternalTransferScreen - Where the user will initiate transfers (only outgoing) from one of his bank account to another user's bank account (he should be able to choose only the general bank account). And the recipient name should be shown, not the pubkey of their main bank account. But inside of the App, we will use the pubkey, ofc
        - InternalTransferScreen - Here the user can transfer funds between his accounts
- models - The database models (classes). In them, the database data is loaded. And their fields correspond 1:1 with the ones from the database
    - user - Class, with attributes -> name(string), age(uint), phone_number(string), wallet_address(string). Its relations should be lazy loaded, and they are - accounts, events
    - account - Class, with attrs -> pub_key(string), token_symbol (always LEV), seed_index(the id of the account), pda_pub_key: (string), ata_pubkey(stirng), account_name(string|null)
    - events - Class, with attrs -> event_id(string/uuid/), type(string), pda_index_from(string|null), pda_index_to(stirng|null), amount(bigint), timestamp(bigint)



### Bank Accounts
The user can create many bank accounts (however he wishes), but, there should always be one GENERAL account. In essence, the bank account with id = 1 is always considered the main bank account.

### Event Types
The event types that I want us to implement are - deposited, withdrawn, transfered_i(nternal), transfered_e(xternal)

## Relay API
A stateless API with the sole purpose of serving as a mediator between the solana program and the tApp (terminal application).

### Headers.
Upon every single request, the API should expect these headers to be set:
- X-PRIV-KEY: <user's private key>
- X-PUB-KEY: <user's public key>

### Endpoints
These endpoints will be exposed to the TUI
- /bankAccount
    - /craete - creates BA (Bank Account)
    - /rename - renames BA
    - /delete - deletes BA(has to point out the transfer account, if the account contains tokens. Otherwise, move to the general account)
- /transfer
    - /internal - internal transfers by user-owned accounts
    - /external - external transfers (only outgoing from the user's POV)
- /deposit - deposit money to BA (BA must be chosen /general by default/) 
- /withdraw - withdraw money from account (BA must be chosen /general by default/

## Solana Program


## External Deps

### TUI
- For the TUI, we will use the library - https://github.com/ArthurSonzogni/ftxui
- Some SQLite adaptor/connector
- No ORM required for the current time being


### Relay
- Express
- TypeScript
- PNPM (for a package manager)
- Solana KIT + Solana SPL

### Onchain
- Anchor
- Anchor_spl
