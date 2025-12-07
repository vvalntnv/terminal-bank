use crate::constants::USER_ACCOUNT_SEED;
use anchor_lang::prelude::*;
use anchor_spl::{
    associated_token::AssociatedToken,
    token::Token,
    token_interface::{Mint, TokenAccount},
};

use crate::errors::*;
use crate::state::*;

pub fn _initialize_user_subaccount(
    ctx: Context<InitializeUserSubaccount>,
    index: u8,
    account_name: String,
) -> Result<()> {
    // initiailze the user's subbaccount
    let user = &ctx.accounts.user;
    let user_account_pda = &mut ctx.accounts.user_account_pda;

    if account_name.len() > 32 {
        return err!(InitializeAccountError::NameTooLong);
    }

    user_account_pda.name = account_name;
    user_account_pda.index = index;
    user_account_pda.owner = user.key();
    user_account_pda.bump = ctx.bumps.user_account_pda;

    Ok(())
}

#[derive(Accounts)]
#[instruction(index: u8)]
pub struct InitializeUserSubaccount<'info> {
    /// The wallet/user creating this bank subaccount.
    #[account(mut)]
    pub user: Signer<'info>,

    /// The new PDA that will store metadata for this subaccount.
    /// Seeds: ["user-account", user_pubkey, index]
    #[account(
        init,
        payer = user,
        space = 8 + UserAccount::MAX_SIZE,
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[index]],
        bump
    )]
    pub user_account_pda: Account<'info, UserAccount>,

    /// ATA owned by the PDA for holding LEV tokens.
    #[account(
        init,
        payer = user,
        associated_token::mint = lev_mint,
        associated_token::authority = user_account_pda
    )]
    pub user_ata: InterfaceAccount<'info, TokenAccount>,

    #[account(
        owner = Token::id(),
    )]
    pub lev_mint: InterfaceAccount<'info, Mint>,
    pub token_program: Program<'info, Token>,

    pub system_program: Program<'info, System>,
    pub associated_token_program: Program<'info, AssociatedToken>,
}
