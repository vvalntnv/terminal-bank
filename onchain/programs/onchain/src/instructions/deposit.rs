use crate::constants::{MINT_AUTH_SEED, USER_ACCOUNT_SEED};
use crate::state::UserAccount;
use anchor_lang::prelude::*;
use anchor_spl::token::mint_to;
use anchor_spl::{
    token::{MintTo, Token},
    token_interface::{Mint, TokenAccount},
};

pub fn _deposit(ctx: Context<Deposit>, index: u8, amount: u64) -> Result<()> {
    let _ = index;

    let token_program = &ctx.accounts.token_program;
    let mint = &ctx.accounts.lev_mint;
    let mint_auth = &ctx.accounts.mint_authority_pda;
    let user_ata = &ctx.accounts.user_ata;

    let bump = ctx.bumps.mint_authority_pda;
    let mint_auth_seeds: &[&[&[u8]]] = &[&[MINT_AUTH_SEED, &[bump]]];

    // mint new tokens
    mint_to(
        CpiContext::new_with_signer(
            token_program.to_account_info(),
            MintTo {
                mint: mint.to_account_info(),
                to: user_ata.to_account_info(),
                authority: mint_auth.to_account_info(),
            },
            mint_auth_seeds,
        ),
        amount,
    )?;

    Ok(())
}

#[derive(Accounts)]
#[instruction(index: u8)]
pub struct Deposit<'info> {
    /// The user authorizes the deposit into one of their PDAs.
    #[account(mut)]
    pub user: Signer<'info>,

    /// PDA that owns the destination ATA.
    #[account(
        mut,
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[index]],
        bump = user_account_pda.bump
    )]
    pub user_account_pda: Account<'info, UserAccount>,

    /// Destination ATA for this subaccount.
    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = user_account_pda
    )]
    pub user_ata: InterfaceAccount<'info, TokenAccount>,

    /// Mint for LEV token.
    #[account(mut)]
    pub lev_mint: InterfaceAccount<'info, Mint>,

    /// Program-derived mint authority PDA.
    /// CHECK: This PDA is used as the mint authority and is validated by seeds.
    #[account(
        seeds = [MINT_AUTH_SEED],
        bump
    )]
    pub mint_authority_pda: UncheckedAccount<'info>,

    pub token_program: Program<'info, Token>,
}
