import { useState, useEffect, useCallback, useRef } from 'react';
import type { AnyMap } from 'react-native-nitro-modules';
import type { Atom, ReadonlyAtom, SetterFn } from '../types';

/**
 * useAtom - Subscribe to an atom and get both value and setter
 *
 * @example
 * ```tsx
 * const [count, setCount] = useAtom(countAtom);
 * ```
 */
export function useAtom<T extends AnyMap>(atom: Atom<T>): [T, SetterFn<T>] {
  const [value, setValue] = useState<T>(() => atom.get());
  const atomRef = useRef(atom);
  atomRef.current = atom;

  useEffect(() => {
    // Sync initial value
    setValue(atomRef.current.get());

    // Subscribe to changes
    const unsubscribe = atomRef.current.subscribe(() => {
      setValue(atomRef.current.get());
    });

    return unsubscribe;
  }, [atom.key]);

  const setter = useCallback<SetterFn<T>>((valueOrUpdater) => {
    atomRef.current.set(valueOrUpdater);
  }, []);

  return [value, setter];
}

/**
 * useAtomValue - Subscribe to an atom and get only the value (read-only)
 *
 * Use this when you only need to read the atom value.
 *
 * @example
 * ```tsx
 * const count = useAtomValue(countAtom);
 * const double = useAtomValue(doubleAtom); // computed
 * ```
 */
export function useAtomValue<T extends AnyMap>(
  atom: Atom<T> | ReadonlyAtom<T>
): T {
  const [value, setValue] = useState<T>(() => atom.get());
  const atomRef = useRef(atom);
  atomRef.current = atom;

  useEffect(() => {
    // Sync initial value
    setValue(atomRef.current.get());

    // Subscribe to changes (only for mutable atoms)
    if ('subscribe' in atomRef.current) {
      const unsubscribe = (atomRef.current as Atom<T>).subscribe(() => {
        setValue(atomRef.current.get());
      });
      return unsubscribe;
    }

    return undefined;
  }, [atom.key]);

  return value;
}

/**
 * useSetAtom - Get only the setter for an atom (no subscription)
 *
 * Use this when you only need to write to the atom,
 * avoiding unnecessary re-renders.
 *
 * @example
 * ```tsx
 * const setCount = useSetAtom(countAtom);
 * ```
 */
export function useSetAtom<T extends AnyMap>(atom: Atom<T>): SetterFn<T> {
  const atomRef = useRef(atom);
  atomRef.current = atom;

  return useCallback<SetterFn<T>>((valueOrUpdater) => {
    atomRef.current.set(valueOrUpdater);
  }, []);
}
