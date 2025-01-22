# Buscamines

## Descripció del projecte

Implementació d'un buscamines el qual elimina el factor de sort del joc de buscamines original, fent que sigui un joc purament basat en habilitat. Per aconseguir-ho s'ha implementat un algorisme usant C++ i compilat de manera que es pugui executar al buscador. També s'ha implementat una interfície molt bàsica pel joc usant JavaScript, CSS i html.
Si es vol fer una petita prova del joc, es pot accedir a aquest enllaç: https://buscamines.vercel.app/

## Compilació i execució del projecte

Per compilar el codi C++ s'ha d'exectutar la següent comanda: "emcc game.cc connection.cc algorithm.cc -o game.js -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" --bind -s MODULARIZE=1 -s EXPORT_ES6=1 -sALLOW_MEMORY_GROWTH"
Això no serà necessari ja que el codi ja està compilat perquè el navegador l'executi, i s'importa des de l'arxiu game.js.
Finalment, com es tracta d'una web estàtica només caldrà obrir en un live server l'arxiu index.html i començar a jugar

## Explicació de l'algorisme usat

En comptes d'usar un SAT-solver que seria una solució fàcil i eficient, per desconeixement s'ha volgut implementar un algorisme de backtracking amb poda específic per resoldre aquest problema. A l'arxiu "algorithm.cc" es pot veure que la funció computeProbabilities() calcula les probabilitats i determina si, amb la informació accessible a l'usuari, hi ha caselles segures, caselles amb mina assegurada o caselles indeterminades. Si l'usuari destapés una casella indeterminada, tingués amb mina i a més no quedessin caselles segures, llavors la funció redistributeMines() es cridaria i redistribuiria les mines de manera que l'usuari no perdés la partida.

## Autor

Genís Carretero Ferrete
