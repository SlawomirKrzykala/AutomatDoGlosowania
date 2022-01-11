# AutomatDoGlosowania
Prosty automat do głosowania

Wyświetlacz alfanumeryczny LCD ze sterownikiem HD44780 oraz interfejsem szeregowym I2C (układ PCF8574), przyciski TAK, NIE, WSTRZYMANY, START/KONIEC.
Rozpoczęcie głosowania następuje po naciśnięciu przycisku START/KONIEC, wówczas poprzednie wyniki głosowania są zerowane; następnie kolejne osoby oddają swoje głosy naciskając jeden z trzech przycisków TAK, NIE lub WSTRZYMANY, po naciśnięciu wyświetlany jest oddany głos, przy czym możliwa jest jeszcze jego zmiana w czasie 5 sekund przez naciśnięcie nowego wyboru (każdy kolejny wybór powoduje zmianę wyboru na inny i odczekanie kolejnego czasu 5 sekund); po akceptacji wyboru (upłynął czas 5 sekund) na wyświetlaczu pojawia się informacja o możliwości oddania kolejnego głosu.
Koniec głosowania następuje przez trzykrotne naciśnięcie START/KONIEC, po zakończeniu głosowania wyświetlane są wyniki tj. całkowita liczba oddanych głosów, głosy za, przeciw i wstrzymujące oraz całkowity czas trwania głosowania,
