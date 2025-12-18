import z from "zod";

export const accountBalanceSchema = z.object({
  accountId: z.number().nonnegative(),
});

export type AccountBalanceRequest = z.infer<typeof accountBalanceSchema>;
