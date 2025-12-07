use anchor_lang::prelude::*;

#[error_code]
pub enum InitializeAccountError {
    #[msg("The Account name is too long")]
    NameTooLong,
}

#[error_code]
pub enum BankingError {
    #[msg("Insufficient funds for the amount requested")]
    InsufficientFunds,
    #[msg("Cannot transfer tokens between two accounts with the same id. That is basically the same account")]
    CannotTransferTokensBetweenSameAccounts,
}
