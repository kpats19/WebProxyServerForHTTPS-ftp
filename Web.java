import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;
public class Web {
    static final byte[] HTML_START = new byte[]{'<', 'h', 't', 'm', 'l', '>', '<', 't', 'i', 't', 'l', 'e', '>', 'H', 'T', 'T', 'P', ' ', 'S', 'E', 'R', 'V', 'E', 'R', '<', '/', 't', 'i', 't', 'l', 'e', '>', '<', 'b', 'o', 'd', 'y', '>'}, HTML_END = new byte[]{'<', '/', 'b', 'o', 'd', 'y', '>', '<', '/', 'h', 't', 'm', 'l', '>'};
    static int port = -1, remotePort = -1;
    static byte host_port[], remoteIp[], host[][];
    static InetAddress inetAddress[];
    static ServerSocket serverSocket;
    static Socket client, server;
    /* @author : gm324, kbp57, ksp59, pt258, jmp235 */
    public static void main(String args[]) {
        try {
            if (args.length <= 0) {
                throw new UnknownError("Arguments Missing Use : \"java " + Web.class.getSimpleName() + " port\"");
            } else {
                port = Integer.parseInt(args[0]);
                if (port <= 10000) {
                    throw new UnknownError("Port number must be greater than 10000.");
                }
            }
        } catch (NumberFormatException ex) {
            System.out.println("Port number can not be string\n" + "Use : \"java " + Web.class.getSimpleName() + " port\"");
            ex.printStackTrace();
            System.exit(-1);
        } catch (UnknownError ex) {
            System.out.println(ex.getMessage());
            ex.printStackTrace();
            System.exit(-1);
        }
        System.out.println("stage 1 program by \"kbp57\" listening on port (" + port + ")\nUse Ctrl-c to exit");
        while (1 == 1) {
            init();
            go();
            try {
                serverSocket.close();
            } catch (IOException ex) {
                ex.printStackTrace();
                System.exit(-1);
            }
        }
    }
    /* @author : ksp59*/
    public static void init() {
        try {
            serverSocket = new ServerSocket();
            serverSocket.bind(new InetSocketAddress(InetAddress.getByAddress(new byte[]{0, 0, 0, 0}), port));
            client = serverSocket.accept();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    /* @author : gm324, kbp57, ksp59*/
    public static void go() {
        byte method[] = new byte[7], full_path[] = new byte[8192], buf[] = new byte[70000];
        int space_offsets[] = new int[100], offsets[] = new int[100], requestlength = 0;
        try {
            InputStream isFromClient = client.getInputStream();
            requestlength = isFromClient.read(buf, 0, buf.length);
            if(requestlength>=65536){
                throw new IOException();
            }
            buf = trim(buf);
            System.out.println(new String(trim(buf)));
            for (int i = 0, cnt = 0; i < requestlength; i++) {
                if (buf[i] == 13) {
                    offsets[++cnt] = i;
                }
            }
            for (int i = 0, cnt = 0; i < offsets[1]; i++) {
                if (buf[i] == 32) {
                    space_offsets[cnt++] = i;
                }
            }
            for (int i = 0; i < space_offsets[0]; i++) {
                method[i] = buf[i];
            }
            for (int i = space_offsets[0]; i < space_offsets[1]; i++) {
                full_path[i - space_offsets[0]] = buf[i];
            }
            parse(trim(full_path));
            dnslookup(trim(host[0]));
            generateResponse(200, buf, method, full_path);
        } catch (UnknownHostException uhx) {
            generateResponse(404, "get".getBytes(), "get".getBytes(), "get".getBytes());
            uhx.printStackTrace();
        } catch (IOException ex) {
            generateResponse(400, "get".getBytes(), "get".getBytes(), "get".getBytes());
            ex.printStackTrace();
        }
    }
    /* @author : kbp57, ksp59 */
    public static void dnslookup(byte[] host) throws UnknownHostException {
        inetAddress = InetAddress.getAllByName(new String(host));
        remoteIp = new byte[55];
        byte bytes[] = inetAddress[0].getAddress();
        for (int j = 0; j < bytes.length; j++) {
            if (j > 0) {
                remoteIp = mergeBytes(remoteIp, ".".getBytes());
            }
            if (bytes[j] >= 0) {
                remoteIp = mergeBytes(remoteIp, (bytes[j] + "").getBytes());
            } else {
                remoteIp = mergeBytes(remoteIp, ((bytes[j] + 256) + "").getBytes());
            }
        }
    }
    /* @author : gm324, pt258, jmp235 ,kbp57 */
    public static void parse(byte[] full_path) {
        int slashcount = 0, coloncnt = 0, cs = 0, filepathcnt = 0;
        byte[][] parts = new byte[255][255];
        for (int i = 0; i < full_path.length; i++) {
            if (full_path[i] == 47) {
                slashcount++;
                cs = 0;
            } else {
                parts[slashcount][cs++] = full_path[i];
            }
        }
        host_port = new byte[255];
        host = new byte[3][255];
        if (new String(trim(parts[0])).endsWith(":")) {
            host_port = trim(parts[2]);
            filepathcnt = 3;
            if (slashcount == 2) {
                slashcount += 1;
            }
        } else {
            host_port = trim(parts[0]);
            filepathcnt = 1;
            if (slashcount == 0) {
                slashcount += 1;
            }
        }
        for (int i = filepathcnt; i <= slashcount; i++) {
            parts[i] = trim(parts[i]);
            host[2] = mergeBytes(host[2], "/".getBytes());
            host[2] = mergeBytes(host[2], parts[i]);
        }
        cs = 0;
        for (int i = 0; i < host_port.length; i++) {
            if (host_port[i] == 58) {
                coloncnt++;
                cs = 0;
            } else {
                host[coloncnt][cs++] = host_port[i];
            }
        }
        if (coloncnt == 0) {
            host[1] = "80".getBytes();
        }
    }
    /* @author : gm324, pt258, jmp235*/
    public static void generateResponse(int statusCode, byte[] response, byte[] method, byte[] full_path) {
        try {
            OutputStream osToClient = client.getOutputStream();
            byte[] STATUS = null, CONTENT_LENGTH_START = null, CONTENT_LENGTH_END = null, finalresponse = new byte[65535];
            byte[] TAB = new byte[]{' ', ' '}, BR_TAG = new byte[]{'<', '/', 'b', 'r', '>'};
            byte[] PRE_TAG_START = new byte[]{'<', 'p', 'r', 'e', '>'}, PRE_TAG_END = new byte[]{'<', '/', 'p', 'r', 'e', '>'};
            byte[] PORT_TAG = new byte[]{'P', 'O', 'R', 'T', ' ', ' ', ' ', '=', ' '}, PATH_TAG = new byte[]{'P', 'A', 'T', 'H', ' ', ' ', ' ', '=', ' '};
            byte[] BRACKET_START = new byte[]{'('}, BRACKET_END = new byte[]{')'};
            byte[] FONT_TAG_START = new byte[]{'<', 'f', 'o', 'n', 't', ' ', 'c', 'o', 'l', 'o', 'r', '=', '\'', 'r', 'e', 'd', '\'', '>', '<', 'b', 'r', '/', '>', 'H', 'O', 'S', 'T', 'I', 'P', ' ', '=', ' '}, FONT_TAG_END = new byte[]{'<', '/', 'f', 'o', 'n', 't', '>'};
            byte[] SERVER_CONFIG = new byte[]{'B', 'a', 's', 'i', 'c', ' ', 'J', 'a', 'v', 'a', ' ', 'S', 'e', 'r', 'v', 'e', 'r', ' ', 't', 'o', ' ', 'P', 'r', 'o', 'c', 'e', 's', 's', ' ', 'H', 'T', 'T', 'P'};
            byte[] CONTENT_TYPE = new byte[]{'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', 't', 'e', 'x', 't', '/', 'h', 't', 'm', 'l', ';', ' ', 'c', 'h', 'a', 'r', 's', 'e', 't', '=', 'U', 'T', 'F', '-', '8'};
            if (new String(method)
                    .trim().equalsIgnoreCase("get")) {
                if (statusCode == 200) {
                    STATUS = new byte[]{'H', 'T', 'T', 'P', '/', '1', '.', '1', ' ', '2', '0', '0', ' ', 'O', 'K', '\\', 'r', '\\', 'n'};
                    byte[] red = new byte[8192];
                    red = mergeBytes(red, FONT_TAG_START);
                    red = mergeBytes(red, host[0]);
                    red = mergeBytes(red, TAB);
                    red = mergeBytes(red, BRACKET_START);
                    red = mergeBytes(red, remoteIp);
                    red = mergeBytes(red, BRACKET_END);
                    red = mergeBytes(red, BR_TAG);
                    red = mergeBytes(red, PORT_TAG);
                    red = mergeBytes(red, host[1]);
                    red = mergeBytes(red, BR_TAG);
                    red = mergeBytes(red, PATH_TAG);
                    red = mergeBytes(red, host[2]);
                    red = mergeBytes(red, FONT_TAG_END);
                    finalresponse = Web.HTML_START;
                    finalresponse = mergeBytes(finalresponse, PRE_TAG_START);
                    finalresponse = mergeBytes(finalresponse, response);
                    finalresponse = mergeBytes(finalresponse, BR_TAG);
                    finalresponse = mergeBytes(finalresponse, red);
                    finalresponse = mergeBytes(finalresponse, PRE_TAG_END);
                    finalresponse = mergeBytes(finalresponse, Web.HTML_END);
                } else if (statusCode == 400) {
                    STATUS = new byte[]{'H', 'T', 'T', 'P', '/', '1', '.', '1', ' ', '4', '0', '0', ' ', 'B', 'a', 'd', ' ', 'R', 'e', 'q', 'u', 'e', 's', 't', '\\', 'r', '\\', 'n'};
                    finalresponse = new byte[]{'B', 'a', 'd', ' ', 'R', 'e', 'q', 'u', 'e', 's', 't', '!'};
                } else if (statusCode == 500) {
                    STATUS = new byte[]{'H', 'T', 'T', 'P', '/', '1', '.', '1', ' ', '5', '0', '0', ' ', 'I', 'n', 't', 'e', 'r', 'n', 'a', 'l', ' ', 'S', 'e', 'r', 'v', 'e', 'r', ' ', 'E', 'r', 'r', 'o', 'r', '\\', 'r', '\\', 'n'};
                    finalresponse = new byte[]{'I', 'n', 't', 'e', 'r', 'n', 'a', 'l', ' ', 's', 'e', 'r', 'v', 'e', 'r', ' ', 'e', 'r', 'r', 'o', 'r', '!'};
                } else {
                    STATUS = new byte[]{'H', 'T', 'T', 'P', '/', '1', '.', '1', ' ', '4', '0', '4', ' ', 'N', 'o', 't', ' ', 'F', 'o', 'u', 'n', 'd', '\\', 'r', '\\', 'n'};;
                    finalresponse = new byte[]{'P', 'a', 'g', 'e', ' ', 'n', 'o', 't', ' ', 'f', 'o', 'u', 'n', 'd'};
                }
            } else {
                STATUS = new byte[]{'H', 'T', 'T', 'P', '/', '1', '.', '1', ' ', '5', '0', '1', ' ', 'N', 'o', 't', ' ', 'I', 'm', 'p', 'l', 'e', 'm', 'e', 'n', 't', 'e', 'd', '\\', 'r', '\\', 'n'};
                finalresponse = new byte[]{'T', 'h', 'i', 's', ' ', 'm', 'e', 't', 'h', 'o', 'd', ' ', 'i', 's', ' ', 'n', 'o', 't', ' ', 'i', 'm', 'p', 'l', 'e', 'm', 'e', 'n', 't', 'e', 'd', '!', '.', '<', '/', 'b', 'r', '>', 'W', 'e', ' ', 'a', 'r', 'e', ' ', 'c', 'u', 'r', 'r', 'e', 'n', 't', 'l', 'y', ' ', 'a', 'c', 'c', 'e', 'p', 't', 'i', 'n', 'g', ' ', 'o', 'n', 'l', 'y', ' ', 'g', 'e', 't', ' ', 'm', 'e', 't', 'h', 'o', 'd', '.'};
            }
            CONTENT_LENGTH_START = new byte[]{'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' '};
            CONTENT_LENGTH_START = mergeBytes(CONTENT_LENGTH_START, (finalresponse.length + "").getBytes());
            CONTENT_LENGTH_END = new byte[]{'\\', 'r', '\\', 'n'};
            CONTENT_LENGTH_START = mergeBytes(CONTENT_LENGTH_START, CONTENT_LENGTH_END);
            System.out.println("\n***************************************" + new String(host[0]) + "\n" + new String(STATUS) + "\n***************************************");
            osToClient.write(STATUS);
            osToClient.write("\r\n".getBytes());
            osToClient.write(SERVER_CONFIG);
            osToClient.write(CONTENT_TYPE);
            osToClient.write(CONTENT_LENGTH_START);
            osToClient.write("Connection: close\r\n\r\n".getBytes());
            osToClient.write("\r\n".getBytes());
            osToClient.write(finalresponse);
            osToClient.flush();
            osToClient.close();
        } catch (IOException ex) {
            ex.printStackTrace();
            generateResponse(500, response, method, full_path);
        }
    }
    /* @author : gm324, kbp57, ksp59, pt258, jmp235 */
    public static byte[] mergeBytes(byte[] first, byte[] second) {
        byte[] merged = new byte[first.length + second.length];
        for (int i = 0; i < merged.length; i++) {
            if (i < first.length) {
                merged[i] = first[i];
            } else {
                merged[i] = second[i - first.length];
            }
        }
        return merged;
    }
    /* @author : gm324, kbp57, ksp59, pt258, jmp235 */
    public static byte[] trim(byte[] trim) {
        int i = trim.length - 1;
        while (true) {
            if (i >= 0 && trim[i] == 0) {
                --i;
            } else {
                break;
            }
        }
        return Arrays.copyOf(trim, i + 1);
    }
}