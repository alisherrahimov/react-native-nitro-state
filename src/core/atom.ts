import type { AnyMap } from 'react-native-nitro-modules';
import { getNitroState } from './instance';
import type {
  Atom,
  ReadonlyAtom,
  SetterFn,
  Getter,
  AtomOptions,
} from '../types';

// Counter for generating unique atom keys
let atomCounter = 0;

/**
 * Create a primitive atom
 *
 * @example
 * ```ts
 * const countAtom = atom(0);
 * const userAtom = atom<User | null>(null);
 * ```
 */
export function atom<T extends AnyMap>(
  initialValue: T,
  options?: AtomOptions<T>
): Atom<T>;

/**
 * Create a derived/computed atom
 *
 * @example
 * ```ts
 * const doubleAtom = atom((get) => get(countAtom) * 2);
 * ```
 */
export function atom<T extends AnyMap>(
  read: (get: Getter) => T,
  options?: AtomOptions<T>
): ReadonlyAtom<T>;

/**
 * Implementation
 */
export function atom<T extends AnyMap>(
  initialValueOrRead: T | ((get: Getter) => T),
  options?: AtomOptions<T>
): Atom<T> | ReadonlyAtom<T> {
  const key = options?.debugLabel ?? `atom_${++atomCounter}`;
  const nitroState = getNitroState();

  // Check if it's a computed atom (function that takes 'get')
  if (typeof initialValueOrRead === 'function') {
    const readFn = initialValueOrRead as (get: Getter) => T;

    // Track dependencies during first read
    const dependencies: string[] = [];

    const getter: Getter = <U extends AnyMap>(
      depAtom: Atom<U> | ReadonlyAtom<U>
    ): U => {
      if (!dependencies.includes(depAtom.key)) {
        dependencies.push(depAtom.key);
      }
      return nitroState.getAtomValue(depAtom.key) as U;
    };

    // Initial computation to discover dependencies
    const compute = () => readFn(getter);

    // Create computed in C++
    nitroState.createComputed(key, dependencies, compute);

    const readonlyAtom: ReadonlyAtom<T> = {
      key,
      get: () => nitroState.getComputedValue(key) as T,
      __atom: true as const,
      __readonly: true as const,
    };

    return readonlyAtom;
  }

  // Primitive atom
  nitroState.createAtom(key, initialValueOrRead);

  const set: SetterFn<T> = (valueOrUpdater) => {
    if (typeof valueOrUpdater === 'function') {
      const updater = valueOrUpdater as (prev: T) => T;
      const currentValue = nitroState.getAtomValue(key) as T;
      const newValue = updater(currentValue);
      nitroState.setAtomValue(key, newValue);
    } else {
      nitroState.setAtomValue(key, valueOrUpdater);
    }
  };

  const primitiveAtom: Atom<T> = {
    key,
    get: () => nitroState.getAtomValue(key) as T,
    set,
    subscribe: (callback: () => void) =>
      nitroState.subscribeAtom(key, callback),
    __atom: true as const,
  };

  return primitiveAtom;
}
