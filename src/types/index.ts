import type { AnyMap } from 'react-native-nitro-modules';

/**
 * Setter function type - accepts value or updater function
 */
export type SetterFn<T extends AnyMap> = (
  valueOrUpdater: T | ((prev: T) => T)
) => void;

/**
 * Getter function for computed atoms
 */
export type Getter = <T extends AnyMap>(atom: Atom<T> | ReadonlyAtom<T>) => T;

/**
 * Atom - Reactive primitive value
 *
 * Stores a value and notifies subscribers when it changes.
 */
export interface Atom<T extends AnyMap> {
  /** Unique identifier */
  readonly key: string;

  /** Get current value */
  get(): T;

  /** Set new value (or update with function) */
  set: SetterFn<T>;

  /** Subscribe to changes, returns unsubscribe function */
  subscribe(callback: () => void): () => void;

  /** Type marker */
  readonly __atom: true;
}

/**
 * ReadonlyAtom - Computed/derived atom (read-only)
 */
export interface ReadonlyAtom<T extends AnyMap> {
  /** Unique identifier */
  readonly key: string;

  /** Get current computed value */
  get(): T;

  /** Type marker */
  readonly __atom: true;
  readonly __readonly: true;
}

/**
 * Options for creating an atom
 */
export interface AtomOptions<T extends AnyMap> {
  /** Custom equality function */
  equals?: (a: T, b: T) => boolean;

  /** Debug label */
  debugLabel?: string;
}

/**
 * Check if value is an Atom
 */
export function isAtom<T extends AnyMap>(value: unknown): value is Atom<T> {
  return (
    typeof value === 'object' &&
    value !== null &&
    '__atom' in value &&
    (value as Atom<T>).__atom === true
  );
}

/**
 * Check if value is a ReadonlyAtom
 */
export function isReadonlyAtom<T extends AnyMap>(
  value: unknown
): value is ReadonlyAtom<T> {
  return (
    isAtom(value) &&
    '__readonly' in (value as object) &&
    (value as any).__readonly === true
  );
}
