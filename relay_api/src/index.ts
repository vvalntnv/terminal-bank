import express from "express";
import helmet from "helmet";
import cors from "cors";
import conf from "./config";
import morgan from "morgan";
import logger from "./utils/logger";

const app = express();

const morganFormat = ":method :url :status :response-time ms";
app.use(
  morgan(morganFormat, {
    stream: {
      write: (message) => {
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

app.get("/", (_, res) => {
  res.json({ status: "OK" }).send();
});

app.listen(conf.port, () => {
  console.log(`Started listening on port ${conf.port}`);
});
