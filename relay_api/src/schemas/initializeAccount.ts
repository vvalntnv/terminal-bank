import { z } from "zod";

export const initializeAccountSchema = z.object({
  userAccountId: z.number().int().min(0),
  userAccountName: z.string().min(1).max(32),
});

export type InitializeAccountSchema = z.infer<typeof initializeAccountSchema>;
