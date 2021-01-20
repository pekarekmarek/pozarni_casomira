# Bezdrátová požární časomíra
Bezdrátová časomíra pro požární sport řízená pomocí platformy Arduino. Obsahuje menu s výběrem možností startu požárního útoku, nastavitelným odpočtem přípravného času a historii naměřených časů uložených na SD kartě.

Menu: 
             | -> Automaticky |             
             |                | -> Spustit | -> Casomira | -> Smazat
Požární útok |                | -> Zpet                  | -> Zpet
             | -> Manualne    |
             |
             | -> Zpet
      
      
Po sepnutí hlavního spínače se zkontroluje správná komunikace se všemi moduly. V případě chybné komunikace se vypíše oznámení na displej. Požární útok nebude možný provést bez úspěšné komunikace s modulem u terčů. V případě, že chybí pouze SD karta nebo RTC modul, útok bude možné provést, ale nebude zaznamenávan. 

Základní menu:

=> Na displeji se zobrazuje šipka, která značí výběr možnosti v momentálním menu. Šipka se může posouvat mezi možnostmi pomocí dvou tlačítek "Nahoru" a "Dolů", výběr se potvrdí stisknutím tlačítka "OK".
=> Na každém menu se nacházi možnost "Zpět" pro návrat na předchozí menu.

Požární útok
- Zde je na výběr způsob startu požárního útoku. Na výber jsou dvě možnosti, automatický způsob startu a manuální. Při potvrzení jedné nebo druhé možnosti se ještě přejde do menu pro potvrzení příslušného způsobu. Potvrzením možnosti "Spustit" se spustí časomíra. V automatickém způsobu startu se ještě před spuštěním časomíry objeví upozornění pro závodníky k připravení na start. Následuje náhodný odpočet a po signalizaci bzučákem je útok odstartován. V manuálním režimu se spustí časomíra hned po potvrzení možnosti "Spustit". Na displeji se zobrazuje momentální čas od spuštění časomíry. Při shození terče se zapíše na displej příslušný teřč a čas jeho shození. Požární útok se dokončí po shození obou terčů. Výsledný čas je ten pomalejší z nich. Na SD kartu se zapíše výsledný čas, časy obou terčů a také datum a čas útoku.
