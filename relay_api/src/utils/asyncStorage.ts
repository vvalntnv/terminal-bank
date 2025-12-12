import { AsyncLocalStorage } from "async_hooks";
import type { RequestContext } from "@/types/requestContext.type.js";

export const asyncLocalStorage = new AsyncLocalStorage<RequestContext>();
