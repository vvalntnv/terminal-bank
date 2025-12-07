use anchor_lang::prelude::*;

#[error_code]
pub enum InitializeAccountError {
    #[msg("The Account name is too long")]
    NameTooLong,
}

#[error_code]
pub enum WithdrawTokensError {
    #[msg("Insufficient funds for the amount requested")]
    InsufficientFunds,
}
