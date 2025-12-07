use crate::constants::USER_ACCOUNT_SEED;
use crate::errors::BankingError;
use crate::state::UserAccount;
use anchor_lang::prelude::*;
use anchor_spl::token::{burn, Burn, Token};
use anchor_spl::token_interface::{Mint, TokenAccount};

pub fn _withdraw(ctx: Context<Withdraw>, index: u8, amount: u64) -> Result<()> {
    let _ = index;

    let user = &ctx.accounts.user;
    let user_ata = &ctx.accounts.user_ata;
    let mint = &ctx.accounts.lev_mint;
    let token_program = &ctx.accounts.token_program;
    let user_pda = &ctx.accounts.user_account_pda;

    if amount > user_ata.amount {
        return err!(BankingError::InsufficientFunds);
    }

    let user_pda_bump = ctx.bumps.user_account_pda;
    let user_key = user.key();

    let signer_seeds: &[&[&[u8]]] = &[&[
        USER_ACCOUNT_SEED,
        user_key.as_ref(),
        &[index],
        &[user_pda_bump],
    ]];

    burn(
        CpiContext::new_with_signer(
            token_program.to_account_info(),
            Burn {
                mint: mint.to_account_info(),
                from: user_ata.to_account_info(),
                authority: user_pda.to_account_info(),
            },
            signer_seeds,
        ),
        amount,
    )?;

    Ok(())
}

#[derive(Accounts)]
#[instruction(index: u8)]
pub struct Withdraw<'info> {
    #[account(mut)]
    pub user: Signer<'info>,

    #[account(
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[index]],
        bump
    )]
    pub user_account_pda: Account<'info, UserAccount>,

    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = user_account_pda
    )]
    pub user_ata: InterfaceAccount<'info, TokenAccount>,

    #[account(mut)]
    pub lev_mint: InterfaceAccount<'info, Mint>,
    pub token_program: Program<'info, Token>,
}
