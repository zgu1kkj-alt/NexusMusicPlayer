# Nexus Music Player

Player desktop local para Windows, escrito em C++20 com Qt 6 Widgets, Qt Multimedia e SQLite.

## Recursos implementados

- Importação de MP3/MP4 em lote e por pasta; arrastar e soltar arquivos na janela.
- Biblioteca persistente SQLite, remoção da biblioteca e detecção de arquivos ausentes.
- Reprodução local real via `QMediaPlayer`: play/pause, anterior/próxima, seek, mute e volume.
- Vídeo MP4 embutido com opção de exibir/ocultar e tela cheia.
- Playlists persistentes, criação, exclusão e adição da faixa selecionada.
- Pesquisa instantânea por título, artista, álbum e nome do arquivo.
- Favoritos e histórico básico de reprodução.
- Tema escuro premium e atalhos de teclado.
- Módulos de integração oficial opcionais para YouTube Data API e Spotify Web API.

## Dependências

- Windows 10/11 x64
- Qt 6.4 ou superior (recomendado Qt 6.8+), módulos: Widgets, Multimedia, MultimediaWidgets, Sql, Network
- CMake 3.21+
- Visual Studio 2022 com workload **Desktop development with C++** (MSVC 2022) ou MinGW compatível com sua instalação do Qt

## Compilar no Windows

1. Instale o Qt pelo Qt Online Installer e selecione o kit MSVC 2022 64-bit, incluindo os módulos acima.
2. Instale CMake e Visual Studio 2022 C++.
3. Abra o **Developer PowerShell for VS 2022**.
4. No diretório do projeto:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\msvc2022_64"
cmake --build build --config Release
```

Ajuste a versão e o caminho para sua instalação real do Qt. O executável será gerado em `build\Release\NexusMusicPlayer.exe`.

Para montar a pasta redistribuível, use o `windeployqt` do mesmo kit Qt:

```powershell
C:\Qt\6.8.3\msvc2022_64\bin\windeployqt.exe build\Release\NexusMusicPlayer.exe
```

O backend multimídia e os codecs disponíveis dependem da versão do Qt, do Windows e dos plugins instalados. MP4 é um contêiner: a reprodução depende dos codecs internos (por exemplo H.264/AAC) e do backend disponível no computador.

## Integrações opcionais

- **YouTube:** informe uma chave da YouTube Data API v3 em Configurações. A integração consulta metadados e abre os resultados no site oficial; não baixa nem extrai mídia.
- **Spotify:** a classe de integração recebe um access token OAuth válido e consulta metadados pela Web API. Configure seu aplicativo no Spotify for Developers e implemente o fluxo OAuth PKCE no produto final antes de distribuir. Não há reprodução/extração de áudio Spotify nesta aplicação.

## Dados

O banco SQLite é criado em `QStandardPaths::AppDataLocation/nexus_library.sqlite`. A biblioteca guarda caminhos locais; se arquivos forem movidos, use **Importar pasta** novamente para registrar o novo caminho. A limpeza de ausentes é oferecida na inicialização.

## Limitações honestas desta versão

- O Qt Multimedia não oferece ganho acima de 100% nem equalizador gráfico universal por meio de `QAudioOutput`; o volume é limitado a 0–100% nesta implementação.
- O visualizador de espectro e o mini player flutuante avançado não estão incluídos nesta primeira entrega.
- Metadados são inicialmente inferidos do nome do arquivo. Extração completa de tags ID3/MP4 e arte incorporada exigiria uma etapa adicional com TagLib/FFmpeg.
- Não foi possível gerar um `.exe` Windows neste ambiente: é necessário compilar com o kit Qt/MSVC no Windows.

## Gerar o executável pelo GitHub Actions

Este repositório inclui `.github/workflows/windows-build.yml`. Envie todos os arquivos do projeto ao GitHub. A compilação inicia automaticamente ao enviar para `main`, ou manualmente em **Actions → Build Nexus Music Player (Windows) → Run workflow**. Quando terminar com sucesso, baixe `NexusMusicPlayer-Windows-x64` na seção **Artifacts** da execução. Extraia o ZIP e execute `NexusMusicPlayer.exe`.
