import { Request, Response, NextFunction } from "express";
import { AppError } from "@/errors/AppError.js";
import logger from "@/utils/logger.js";

export const errorHandler = (
  err: Error,
  req: Request,
  res: Response,
  next: NextFunction
) => {
  if (err instanceof AppError) {
    logger.warn(`AppError: ${err.message} (Status: ${err.statusCode})`);
    return res.status(err.statusCode).json({
      status: "error",
      message: err.message,
    });
  }

  // Unexpected errors
  logger.error(`Unexpected Error: ${err.message}`, { stack: err.stack });
  return res.status(500).json({
    status: "error",
    message: "Something went wrong",
  });
};
