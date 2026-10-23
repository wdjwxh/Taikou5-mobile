package org.nxnative.taikou;

import android.content.Context;
import android.net.Uri;
import org.json.JSONObject;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;
import java.util.zip.*;

/** Imports only personal packages produced by prepare_game.py. Saves live outside replacements. */
final class GameImporter {
    static final long LIMIT = 8L << 30;
    /** Fast startup check; full content hashing is performed once during import. */
    static boolean isReady(File current) {
        try {
            File manifest=new File(current,"game.json");
            if(!manifest.isFile()||manifest.length()>8*1024*1024)return false;
            JSONObject m=new JSONObject(new String(java.nio.file.Files.readAllBytes(manifest.toPath()),java.nio.charset.StandardCharsets.UTF_8));
            if(m.getInt("schema")!=1||!m.getString("title_id").equals("0100346017304000")||!m.getString("main_build_id").equals("f90fdcaf95cce279df79279102062c88"))return false;
            JSONObject files=m.getJSONObject("files");
            Iterator<String> names=files.keys();
            while(names.hasNext()){
                String name=names.next();File file=new File(current,name);
                if(!file.getCanonicalPath().startsWith(current.getCanonicalPath()+File.separator)||!file.isFile()||file.length()!=files.getJSONObject(name).getLong("size"))return false;
            }
            for(String mod:new String[]{"main","subsdk0","sdk"})
                if(!new File(current,"out/"+mod+".patched.bin").isFile()||!new File(current,"out/"+mod+".fixups.tsv").isFile())return false;
            File[] shaders=new File(current,"shaders").listFiles((dir,name)->name.endsWith(".xgs"));
            return new File(current,"RomFS/TDATA/BUSTUP.G1T").isFile()&&shaders!=null&&shaders.length==212;
        } catch(Exception e){return false;}
    }
    static String hex(byte[] bytes) {
        StringBuilder s = new StringBuilder();
        for (byte b : bytes) s.append(String.format(Locale.ROOT,"%02x",b & 255));
        return s.toString();
    }
    static void remove(File file) throws IOException {
        File[] children=file.listFiles();
        if(children!=null) for(File c:children) remove(c);
        if(file.exists()&&!file.delete())throw new IOException("Cannot remove temporary import: "+file.getName());
    }
    static void install(Context context, Uri uri) throws Exception {
        File root=new File(context.getFilesDir(),"game");root.mkdirs();
        File staging=new File(root,"importing"),current=new File(root,"current"),old=new File(root,"previous");
        remove(staging);staging.mkdirs();
        Map<String,String> hashes=new HashMap<>();Map<String,Long> sizes=new HashMap<>();
        long total=0;int count=0;
        try {
            InputStream raw=context.getContentResolver().openInputStream(uri);
            if(raw==null)throw new IOException("Cannot open selected document");
            try(ZipInputStream zip=new ZipInputStream(new BufferedInputStream(raw))) {
                ZipEntry entry;byte[] buffer=new byte[262144];
                while((entry=zip.getNextEntry())!=null) {
                    String name=entry.getName();
                    if(entry.isDirectory())continue;
                    if(++count>30000||name.contains("\\")||name.startsWith("/")||name.contains(":")||Arrays.asList(name.split("/",-1)).contains(".."))throw new IOException("Unsafe import path");
                    if(!(name.equals("game.json")||name.startsWith("out/")||name.startsWith("RomFS/")||name.startsWith("shaders/")))throw new IOException("Unexpected file: "+name);
                    if(hashes.containsKey(name))throw new IOException("Duplicate file: "+name);
                    File target=new File(staging,name);
                    if(!target.getCanonicalPath().startsWith(staging.getCanonicalPath()+File.separator))throw new IOException("Unsafe import path");
                    if(!target.getParentFile().isDirectory()&&!target.getParentFile().mkdirs())throw new IOException("Cannot create import directory");
                    MessageDigest hash=MessageDigest.getInstance("SHA-256");long size=0;
                    try(OutputStream out=new BufferedOutputStream(new FileOutputStream(target))) {
                        int n;while((n=zip.read(buffer))!=-1){total+=n;size+=n;if(total>LIMIT||(name.equals("game.json")&&size>8*1024*1024))throw new IOException("Import exceeds size limit");out.write(buffer,0,n);hash.update(buffer,0,n);}
                    }
                    hashes.put(name,hex(hash.digest()));sizes.put(name,size);
                }
            }
            File manifest=new File(staging,"game.json");
            if(!manifest.isFile())throw new IOException("Missing game.json; select a generated personal game ZIP");
            JSONObject m=new JSONObject(new String(java.nio.file.Files.readAllBytes(manifest.toPath()),java.nio.charset.StandardCharsets.UTF_8));
            if(m.getInt("schema")!=1||!m.getString("title_id").equals("0100346017304000")||!m.getString("main_build_id").equals("f90fdcaf95cce279df79279102062c88"))throw new IOException("Unsupported game build; prepare the documented v196608 update");
            JSONObject files=m.getJSONObject("files");
            if(files.length()!=hashes.size()-1)throw new IOException("Import file count differs from manifest");
            Iterator<String> names=files.keys();while(names.hasNext()){
                String name=names.next();JSONObject f=files.getJSONObject(name);
                if(!f.getString("sha256").equals(hashes.get(name))||!Objects.equals(sizes.get(name),f.getLong("size")))throw new IOException("File verification failed: "+name);
            }
            for(String mod:new String[]{"main","subsdk0","sdk"}) {
                if(!new File(staging,"out/"+mod+".patched.bin").isFile()||!new File(staging,"out/"+mod+".fixups.tsv").isFile())throw new IOException("Missing rewritten module: "+mod);
                String fix=new String(java.nio.file.Files.readAllBytes(new File(staging,"out/"+mod+".fixups.tsv").toPath()),java.nio.charset.StandardCharsets.UTF_8);
                if(!fix.contains("# TARGET\t0x1"))throw new IOException("Module is not rewritten for Android: "+mod);
            }
            if(!new File(staging,"RomFS/TDATA/BUSTUP.G1T").isFile())throw new IOException("Missing RomFS marker");
            File[] shaders=new File(staging,"shaders").listFiles((dir,name)->name.endsWith(".xgs"));
            if(shaders==null||shaders.length!=212)throw new IOException("Expected 212 translated shaders");
            remove(old);
            if(current.exists()&&!current.renameTo(old))throw new IOException("Cannot preserve previous import");
            if(!staging.renameTo(current)){if(old.exists())old.renameTo(current);throw new IOException("Cannot activate import");}
            new File(root,"save").mkdirs();
            // Failure to reclaim old resources must not invalidate the newly verified current import.
            try{remove(old);}catch(IOException ignored){}
        } catch(Exception e){try{remove(staging);}catch(IOException ignored){}throw e;}
    }
}
