import express from "express";
import helmet from "helmet";
import cors from "cors";
import conf from "@/config.js";
import morgan from "morgan";
import logger from "@/utils/logger.js";
import { errorHandler } from "@/middleware/errorHandler.js";
import { extractWalletDataMiddleware } from "./middleware/extractWallet.js";
import { NotFoundError } from "@/errors/generalErrors.js";
import { asyncLocalStorage } from "./utils/asyncStorage.js";

const app = express();

const morganFormat = ":method :url :status :response-time ms";
app.use(
  morgan(morganFormat, {
    stream: {
      write: (message: string) => {
        const logObject = {
          method: message.split(" ")[0],
          url: message.split(" ")[1],
          status: message.split(" ")[2],
          responseTime: message.split(" ")[3],
        };
        logger.http(JSON.stringify(logObject));
      },
    },
  }),
);

app.use(helmet());
app.use(cors());
// Always require the wallet
app.use(extractWalletDataMiddleware);

app.get("/", (_, res) => {
  res.json({ status: "OK" }).send();
});

app.get("/address", (_, res) => {
  const wallet = asyncLocalStorage.getStore()?.keypair;

  res.json({ address: wallet?.address }).send();
});

// Handle 404
app.use((req, _, next) => {
  next(new NotFoundError(`Route ${req.originalUrl} not found`));
});

app.use(errorHandler);

app.listen(conf.port, () => {
  console.log(`Started listening on port ${conf.port}`);
});
