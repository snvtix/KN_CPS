from gnuradio import gr
from gnuradio import analog
from gnuradio import blocks

class MyTopBlock(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        # Parametry sygnału
        amplitude = 1
        frequency = 5128
        sample_rate = 2e6  # 2 MHz

        # Tworzenie generującego sygnał bloku
        source = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, frequency, amplitude)

        # Tworzenie wektora z informacją (0 i 1)
        pre = [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*780 + [1]*750 + [0]*7660

        f = open("input.txt", "r")
        input = list(f.read())
        pom = []
        i = 1
        while i <= 98:
                pom.append(int(input[i]))
                i = i + 3
                if i == 98:
                        break
        input_vec = [1]*750
        if pom[0] == 0:
                input_vec = input_vec + [0]*780
        elif pom[0] == 1:
                input_vec = input_vec + [1]*750

        j = 1
        while True:
                input_vec = input_vec + [0]*780 + [1]*750
                if pom[j] == 0:
                        input_vec = input_vec + [0]*780
                elif pom[j] == 1:
                        input_vec = input_vec + [1]*750
                j = j + 1
                if j == 32:
                        break

        const = [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520 + [0]*780 + [1]*750 + [0]*1540 + [1]*1520
        information_vector = pre + input_vec + const
        with open('gen_out_vec.txt', 'w') as f:
                f.write(str(information_vector))

        # Tworzenie bloku mnożącego przez stałą z początkową wartością 1
        multiply_block = blocks.multiply_ff()

        # Tworzenie strumienia danych z wektora informacji
        information_stream = blocks.vector_source_f(information_vector, False, 1, [])

        # Tworzenie bloku ograniczającego liczbę próbek
        head_block = blocks.head(gr.sizeof_float, len(information_vector))

        # Tworzenie bloku zapisującego do pliku CSV
        file_sink = blocks.file_sink(gr.sizeof_float, 'o.raw', False)

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

