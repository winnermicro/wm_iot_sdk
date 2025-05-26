# HTTP Client

## Function Overview
This example demonstrates how to use the http client component to access cloud-based large language models (such as Doubao, Deepseek) through AT Commands.

## Environment Requirements

1. You need an AP router or a mobile hotspot that can connect to the Internet.
2. You need to have your own API KEY for the corresponding large language model and replace the relevant macro definitions in the `llm_cli_cmd.c` file. For example: `WM_LLM_DS_KEY`, `WM_LLM_DS_MODEL`. You can find out how to apply for the API KEY of the corresponding large language model on its official website.

## Compilation and Flashing

Example location: `examples/ref_design/ai_llm`

For compilation, flashing and other operations, please refer to: [Quick Start](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Notes

1. The default API KEY in this project may expire, preventing connection to the large language model server. Users are required to apply for a valid API KEY and replace the corresponding macro definitions in the code.
2. This project uses the CLI component for user information input. The length is set by the `CONFIG_CLI_LINE_SIZE` macro definition. Currently, the project has adjusted it to the default value of 256 bytes in the `prj.config` file. If you need to input longer information, you can make corresponding adjustments in the `prj.config` file or the `menuconfig` interface. Otherwise, if the information exceeds the length, the system will prompt `Warning: Command is too long`.
3. You need to configure the WiFi SSID and WiFi Password of your router in the `menuconfig`. After the W80X system is powered on, it will try to automatically connect to this router.
4. Currently, the system integrates the deepseek-chat large language model and the Doubao's doubao-lite-128k-240828 large language model. Their data communication uses the HTTP + CJSON transmission method and content format. For example, the curl format of Doubao is as follows. This project mainly constructs its HTTP Client Header and cJSON format data content, and sends messages to the server specified by the URL through the HTTP Client API.
```
curl https://ark.cn-beijing.volces.com/api/v3/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer 8eccdf3b-3012-4418-9e54-93af922decad" \
  -d '{
    "model": "doubao-lite-128k-240828",
    "messages": [
      {"role": "user","content": "常见的十字花科植物有哪些？"}
    ]
  }'
```

## Running Result

1. After the W80X system is powered on, you can input the `llm` string command through the serial port to get the command description.
```
llm

Usage
  llm [options]

Options:
  ds <contents>   send contents to deepseek llm, max len < CONFIG_CLI_LINE_SIZE Bytes
  db <contents>   send contents to doubao llm, max len < CONFIG_CLI_LINE_SIZE Bytes
  help            print help and exit

W800: 
```
2. You can continue to input information to interact with the large language model according to the command description. For example, input `llm db Write a poem about summer`.

If it runs successfully, the following log will be output:
```
W800: 
W800: llm db Write a poem about summer
[I] (1198428) llm: WM_HTTP_CLIENT_EVENT_CONNECTED
[I] (1198429) llm: WM_HTTP_CLIENT_EVENT_HEADER_SENTED
[I] (1202686) llm: WM_HTTP_CLIENT_EVENT_RECV_HEADER
[I] (1202687) llm: Start to receive data from remote server...
[I] (1202687) llm: WM_HTTP_CLIENT_EVENT_RECV_DATA
[I] (1202687) llm: 

【LLM Answer】:
assistant:  **"Summer's Embrace"**

In the midst of a sun-drenched sky,
Summer spreads her vibrant lullaby.
The warm breeze dances with glee,
Whispering tales of lazy days free.

Fields of golden wheat wave in the glow,
Like a sea of dreams, it doth bestow.
The scent of blooming flowers fills the air,
A fragrant symphony, everywhere.

By the shimmering lake's edge we stand,
Feet dipped in water, cool and grand.
The sun's rays paint the world in gold,
Summer's touch, a magic hold.

Kids laugh and play under the sun,
Their spirits high, like shooting stars.
Picnics by the grassy lawn,
Summer memories, forever drawn.

Evening comes with soft hues,
The sky a canvas painted anew.
Summer's end nears, but her charm lingers still,
In our hearts, a season's thrill. 
[I] (1202690) llm: 

【LLM Answer End】:
[I] (1202690) llm: WM_HTTP_CLIENT_EVENT_DISCONNECTED

W800: 
```