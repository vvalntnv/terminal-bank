mod constants;
mod errors;
mod instructions;
mod state;

use anchor_lang::prelude::*;
use instructions::*;

declare_id!("CJ5ysHVQzRNQqSDoPekKhhyq1i11Wn15WbUhKRZYScqg");

#[program]
pub mod onchain {
    use super::*;

    pub fn initialize(
        ctx: Context<InitializeUserSubaccount>,
        index: u8,
        account_name: String,
    ) -> Result<()> {
        instructions::_initialize_user_subaccount(ctx, index, account_name)
    }

    pub fn deposit(ctx: Context<Deposit>, index: u8, amount: u64) -> Result<()> {
        instructions::_deposit(ctx, index, amount)
    }

    pub fn withdraw(ctx: Context<Withdraw>, index: u8, amount: u64) -> Result<()> {
        instructions::_withdraw(ctx, index, amount)
    }

    pub fn internal_transfer(
        ctx: Context<InternalTransfer>,
        sender_index: u8,
        receiver_index: u8,
        amount: u64,
    ) -> Result<()> {
        instructions::internal_transfer(ctx, sender_index, receiver_index, amount)
    }
}
