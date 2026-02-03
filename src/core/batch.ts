import { getNitroState } from './instance';

/**
 * Batch multiple atom updates
 *
 * All updates within the callback are batched together,
 * and subscribers are notified only once at the end.
 *
 * @example
 * ```ts
 * batch(() => {
 *   countAtom.set(1);
 *   nameAtom.set('John');
 *   // Subscribers notified once here
 * });
 * ```
 */
export function batch(callback: () => void): void {
  const nitroState = getNitroState();

  nitroState.startBatch();
  try {
    callback();
  } finally {
    nitroState.endBatch();
  }
}
