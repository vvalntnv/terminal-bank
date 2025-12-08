use crate::errors::BankingError;
use crate::{constants::USER_ACCOUNT_SEED, state::UserAccount};
use anchor_lang::prelude::*;
use anchor_spl::token_interface::{Mint, TokenAccount};
use anchor_spl::token::{transfer, Transfer, Token};

pub fn _external_transfer(ctx: Context<ExternalTransfer>, sender_account_id: u8, receiver_address: Pubkey, amount: u64) -> Result<()> {
    let _ = receiver_address;
    
    let token_program = &ctx.accounts.token_program;

    let sender_ata = &ctx.accounts.sender_ata;
    let receiver_ata = &ctx.accounts.receiver_ata_account;

    let sender_pda = &ctx.accounts.sender_pda_account;
    let user = &ctx.accounts.user;
    let user_key = user.key();

    if amount > sender_ata.amount {
        return err!(BankingError::InsufficientFunds);
    }

    if user.key() == receiver_address {
        return err!(BankingError::CannotTransferTokensBetweenSameAccounts);
    }

    let sender_seeds: &[&[&[u8]]] = &[
        &[
            USER_ACCOUNT_SEED,
            user_key.as_ref(),
            &[sender_account_id],
            &[ctx.bumps.sender_pda_account]
        ]
    ];

    transfer(
        CpiContext::new_with_signer(
            token_program.to_account_info(),
            Transfer { from: sender_ata.to_account_info() , to: receiver_ata.to_account_info() , authority: sender_pda.to_account_info() },
            sender_seeds
        ),
        amount
    )?;
 
    Ok(())
}

#[derive(Accounts)]
#[instruction(sender_account_id: u8, receiver_address: Pubkey)]
pub struct ExternalTransfer<'info> {
    #[account(mut)]
    pub user: Signer<'info>,

    pub lev_mint: InterfaceAccount<'info, Mint>,

    #[account(
        seeds = [USER_ACCOUNT_SEED, user.key().as_ref(), &[sender_account_id]],
        bump
    )]
    pub sender_pda_account: Account<'info, UserAccount>,

    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = sender_pda_account 
    )]
    pub sender_ata: InterfaceAccount<'info, TokenAccount>,

    // Transfers are always done to the user general account (account with id = 1)
    #[account(
        seeds = [USER_ACCOUNT_SEED, receiver_address.as_ref(), &[1 as u8]],
        bump,
    )]
    pub receiver_pda_account: Account<'info, UserAccount>,

    #[account(
        mut,
        associated_token::mint = lev_mint,
        associated_token::authority = receiver_pda_account
    )]
    pub receiver_ata_account: InterfaceAccount<'info, TokenAccount>,

    pub token_program: Program<'info, Token>
}
