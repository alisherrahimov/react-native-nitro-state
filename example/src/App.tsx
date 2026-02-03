import { Text, View, StyleSheet, Button } from 'react-native';
import { atom, useAtom } from 'react-native-nitro-state';

const countAtom = atom({ count: 0 });

export default function App() {
  const [state, setState] = useAtom(countAtom);

  return (
    <View style={styles.container}>
      <Text style={styles.text}>Count: {state.count}</Text>
      <View style={styles.buttonContainer}>
        <Button
          title="Increment"
          onPress={() => setState((prev) => ({ count: prev.count + 1 }))}
        />
        <Button title="Reset" onPress={() => setState({ count: 0 })} />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#fff',
  },
  text: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 20,
  },
  buttonContainer: {
    gap: 10,
  },
});
