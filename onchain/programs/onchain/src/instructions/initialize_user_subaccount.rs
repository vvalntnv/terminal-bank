use anchor_lang::prelude::*;

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
    pub user_ata: InterfaceAccount<'info, anchor_spl::token::TokenAccount>,

    #[account(
        owner = anchor_spl::token::Token::id(),
    )]
    pub lev_mint: InterfaceAccount<'info, anchor_spl::token::Mint>,

    pub system_program: Program<'info, System>,
    pub associated_token_program: Program<'info, anchor_spl::associated_token::AssociatedToken>,
    pub token_program: Program<'info, anchor_spl::token_interface::TokenInterface>,
    pub rent: Sysvar<'info, Rent>,
}
