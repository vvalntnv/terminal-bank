import { z, ZodSchema } from "zod";
import { Request, Response, NextFunction } from "express";
import { BadRequestError } from "@/errors/generalErrors.js";

export const validate =
  (schema: ZodSchema) => (req: Request, _res: Response, next: NextFunction) => {
    try {
      req.body = schema.parse(req.body);
      next();
    } catch (error) {
      if (error instanceof z.ZodError) {
        const errorMessages = error.issues.map((issue) => ({
          message: `${issue.path.join(".")} is ${issue.message}`,
        }));
        throw new BadRequestError(
          `Validation failed: ${JSON.stringify(errorMessages)}`,
        );
      }
      throw new BadRequestError("Invalid request data");
    }
  };
