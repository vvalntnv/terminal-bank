import z from "zod";

export const airdropSchema = z.object({
  amountToAirdrop: z.number().nonnegative(),
});

export type AirdropType = z.infer<typeof airdropSchema>;
