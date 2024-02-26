from gnuradio import gr
from gnuradio import analog
from gnuradio import blocks

class MyTopBlock(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        # Parametry sygnału
        amplitude = 1
        frequency = 2850  # 1 MHz
        sample_rate = 2e6  # 2 MHz

        # Tworzenie generującego sygnał bloku
        source = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, frequency, amplitude)

        # Tworzenie wektora z informacją (0 i 1)
        information_vector = [1] * 700 + [0] * 700 + [0] * 700 + [1] * 700

        # Tworzenie bloku mnożącego przez stałą z początkową wartością 1
        multiply_block = blocks.multiply_ff()

        # Tworzenie strumienia danych z wektora informacji
        information_stream = blocks.vector_source_f(information_vector, False, 1, [])

        # Tworzenie bloku ograniczającego liczbę próbek
        head_block = blocks.head(gr.sizeof_float, len(information_vector))

        # Tworzenie bloku zapisującego do pliku CSV
        file_sink = blocks.file_sink(gr.sizeof_float, 'o.bin', False)

        # Łączenie bloków
        self.connect(source, (multiply_block,0))
        self.connect(information_stream, (multiply_block,1))
        self.connect(multiply_block, head_block)
        self.connect(head_block, file_sink)

if __name__ == '__main__':
    try:
        # Uruchamianie top_block
        tb = MyTopBlock()
        tb.run()
    except KeyboardInterrupt:
        # Zatrzymywanie programu po naciśnięciu Ctrl+C
        pass

