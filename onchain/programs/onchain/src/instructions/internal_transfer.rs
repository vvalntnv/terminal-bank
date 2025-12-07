use crate::{
    constants::USER_ACCOUNT_SEED,
    state::UserAccount,
};

use crate::errors::BankingError;
use anchor_spl::{token::{Transfer, transfer}, token_interface::{Mint, TokenAccount}};
use anchor_spl::token::Token;
use anchor_lang::prelude::*;

pub fn internal_transfer(ctx: Context<InternalTransfer>, sender_index: u8, receiver_index: u8, amount: u64) -> Result<()> {
    if receiver_index == sender_index {
        return err!(BankingError::CannotTransferTokensBetweenSameAccounts);
    }

    let user = &ctx.accounts.user;
    let sender_ata = &ctx.accounts.sender_ata;
    let receiver_ata = &ctx.accounts.receiver_ata;
    let token_program = &ctx.accounts.token_program;
    let sender_pda = &ctx.accounts.sender_account_pda;

    if amount > sender_ata.amount {
        return err!(BankingError::InsufficientFunds);
    }

    let sender_pda_bump = ctx.bumps.sender_account_pda;
    let user_key = user.key();

    let signer_seeds: &[&[&[u8]]] = &[&[
        USER_ACCOUNT_SEED,
        user_key.as_ref(),
        &[sender_index],
        &[sender_pda_bump],
    ]];

    transfer(CpiContext::new_with_signer(
        token_program.to_account_info(),
        Transfer {
            from: sender_ata.to_account_info(),
            to: receiver_ata.to_account_info(),
            authority: sender_pda.to_account_info(),
        },
        signer_seeds
    ), amount)?;


    Ok(())
}

#[derive(Accounts)]
#[instruction(sender_index: u8, receiver_index: u8)]
pub struct InternalTransfer<'info> {
    #[account(mut)]
    pub user: Signer<'info>,

    #[account(mut)]
    pub lev_mint: InterfaceAccount<'info, Mint>,

    #[account(
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[sender_index]],
        bump
    )]
    pub sender_account_pda: Account<'info, UserAccount>,

    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = sender_account_pda 
    )]
    pub sender_ata: InterfaceAccount<'info, TokenAccount>,

    #[account(
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[receiver_index]],
        bump
    )]
    pub receiver_account_pda: Account<'info, UserAccount>,

    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = receiver_account_pda 
    )]
    pub receiver_ata: InterfaceAccount<'info, TokenAccount>,

    pub token_program: Program<'info, Token>
}
