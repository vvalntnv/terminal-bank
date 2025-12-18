# Agent Guidelines for Terminal Bank

## Commands
- **Onchain** (`/onchain`):
  - Test All: `anchor test`
  - Single Test: `yarn run ts-mocha -p ./tsconfig.json -t 1000000 "tests/<file>.ts"`
  - Lint: `yarn lint:fix`
- **Relay API** (`/relay_api`):
  - Dev: `pnpm dev`
  - Test All: `pnpm test`
  - Single Test: `NODE_ENV=test pnpm exec mocha tests/<file>.ts`

## Code Style & Conventions
- **TypeScript**:
  - Use `camelCase` for vars/funcs, `PascalCase` for classes/types.
  - Prefer `async/await` over chains. Explicit return types.
  - Imports: Group external first, then internal. Use absolute paths or aliases if configured.
  - **Relay API**: Stateless. Use `AppError` for exceptions.
- **Rust (Anchor)**:
  - Standard `snake_case` for functions/vars.
  - Follow Anchor patterns (`Context`, `Accounts`, `PDAs`).
  - Run `cargo fmt` before committing.
- **General**:
  - No secrets in code.
  - verify changes with tests before finishing.
