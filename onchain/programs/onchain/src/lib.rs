mod errors;
mod instructions;
mod state;
mod constants;

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
}
