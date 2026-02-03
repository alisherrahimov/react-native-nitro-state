# React Native Nitro State - Development Rules

## Project Overview

This is a **fine-grained state management** library for React Native, built on top of [Nitro Modules](https://nitro.margelo.com/) for maximum performance using C++/native bindings.

---

## Core Principles

### 1. Code Quality Standards

- **TypeScript Strict Mode**: Always use strict TypeScript with no `any` types unless absolutely necessary
- **Explicit Return Types**: All functions must have explicit return type annotations
- **Immutability**: Prefer `const` over `let`, use readonly arrays and objects where applicable
- **Pure Functions**: Favor pure functions without side effects for predictability

### 2. Architecture Patterns

- **Single Responsibility**: Each module/class should have one clear purpose
- **Dependency Injection**: Avoid hard-coded dependencies; use DI for testability
- **Interface-First Design**: Define interfaces before implementations
- **Separation of Concerns**: Keep native specs, business logic, and React bindings separate

### 3. File Structure

```
src/
├── index.tsx                    # Public API exports
├── *.nitro.ts                   # Nitro HybridObject specifications
├── types/                       # Type definitions
├── hooks/                       # React hooks
├── stores/                      # State store implementations
└── utils/                       # Utility functions
```

---

## Naming Conventions

| Type               | Convention                            | Example                         |
| ------------------ | ------------------------------------- | ------------------------------- |
| Files (components) | PascalCase                            | `StateProvider.tsx`             |
| Files (utils)      | camelCase                             | `createStore.ts`                |
| Nitro specs        | PascalCase.nitro.ts                   | `NitroState.nitro.ts`           |
| Interfaces         | PascalCase with `I` prefix (optional) | `StateConfig` or `IStateConfig` |
| Types              | PascalCase                            | `StoreOptions`                  |
| Constants          | SCREAMING_SNAKE_CASE                  | `DEFAULT_CONFIG`                |
| Functions          | camelCase                             | `createAtom()`                  |
| Hooks              | camelCase with `use` prefix           | `useAtom()`                     |

---

## TypeScript Guidelines

### Required Patterns

```typescript
// ✅ Good: Explicit types, readonly, const assertions
export interface StoreConfig<T> {
  readonly initialValue: T;
  readonly key: string;
  readonly persist?: boolean;
}

export const createStore = <T>(config: StoreConfig<T>): Store<T> => {
  // implementation
};

// ❌ Bad: Implicit any, mutable
export const createStore = (config) => {
  // ...
};
```

### Forbidden Patterns

- `any` type (use `unknown` with type guards instead)
- Non-null assertions `!` (use proper null checks)
- Type casting with `as` (use type guards or generics)
- Implicit return types on exported functions

---

## React & Native Module Guidelines

### Hooks Best Practices

```typescript
// ✅ Good: Proper memoization and dependencies
export const useAtomValue = <T>(atom: Atom<T>): T => {
  const [value, setValue] = useState<T>(atom.get());

  useEffect(() => {
    const unsubscribe = atom.subscribe(setValue);
    return unsubscribe;
  }, [atom]);

  return value;
};
```

### Nitro Module Patterns

```typescript
// ✅ Good: Clear interface definition
export interface NitroState extends HybridObject<{
  ios: 'swift';
  android: 'kotlin';
}> {
  // Getters
  getValue<T>(key: string): T | null;

  // Setters (return void for sync, Promise for async)
  setValue<T>(key: string, value: T): void;

  // Subscriptions
  subscribe(key: string, callback: (value: unknown) => void): () => void;
}
```

---

## Native Code Standards

### Swift (iOS)

- Use `final class` for non-inheritable classes
- Prefer value types (`struct`) over reference types where possible
- Use `@MainActor` for UI-related code
- Implement proper error handling with `Result<T, Error>` or throws

### Kotlin (Android)

- Use `data class` for pure data holders
- Prefer `val` over `var` (immutability)
- Use sealed classes for state representation
- Implement proper coroutine scopes for async operations

---

## Performance Requirements

1. **Minimize Bridge Crossings**: Batch operations when possible
2. **Use Native Threading**: Heavy computations should run on native threads
3. **Lazy Initialization**: Initialize expensive resources only when needed
4. **Memory Management**: Properly clean up subscriptions and listeners

---

## Testing Requirements

- **Unit Tests**: All utility functions and store logic
- **Integration Tests**: Native module integration
- **Type Tests**: Ensure proper TypeScript inference

---

## Version Requirements

| Dependency                 | Minimum Version |
| -------------------------- | --------------- |
| React Native               | 0.83.0+         |
| React                      | 19.x            |
| TypeScript                 | 5.9+            |
| react-native-nitro-modules | 0.33.3+         |

---

## Git & Release

- Follow [Conventional Commits](https://www.conventionalcommits.org/)
- Use semantic versioning (SemVer)
- All PRs must pass CI checks before merge
