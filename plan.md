historia undo/redo

trzymanie stanu programu i 20 ostatnich akcji
jak użytkownik wciśnie redo to wczytywany jest ostatni stan i robione k - 1 operacji
gdzie k jest obecnym wskaźnikiem w stosie operacji

przechowywany jest wskaźnik na koniec stosu, end i początek, start
               k
     s         e
stan 0 1 2 3 4 5 6 7 8 9 

jeżeli k = 5 i użykownik wykona akcje to k += 1 i tab[k] to nowa akcja
jeżeli k = 9 to k += 1 (mod size) i tak samo

undo -> zmniejsz k, jesli k = head + 1 to jesteśmy na końcu 

chcę miec stan i akcje ktore pozwalaja mi "skryptowac" tworzenie animacji
dodaj punkt, przesun punkt, dodaj klatke
akcje powinny pamietac rzeczy jak pozycja do ktorej sie przesuwa i id klatki
na ktorej byly robione akcje
