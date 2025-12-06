mod instructions;
mod state;

use anchor_lang::prelude::*;

declare_id!("CJ5ysHVQzRNQqSDoPekKhhyq1i11Wn15WbUhKRZYScqg");

#[program]
pub mod onchain {
    use super::*;

    pub fn initialize(ctx: Context<Initialize>) -> Result<()> {
        msg!("Greetings from: {:?}", ctx.program_id);
        Ok(())
    }
}
