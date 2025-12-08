use anchor_lang::prelude::*;

#[account]
pub struct UserAccount {
    pub owner: Pubkey,
    pub name: String,
    pub index: u8,
    pub bump: u8,
}

impl UserAccount {
    pub const MAX_SIZE: usize = 32 + // owner
        4 + 32 + // name (up to 32 chars)
        1 + // index
        1; // bump
}
