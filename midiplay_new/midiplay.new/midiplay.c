/* midiplay - plays MIDI file through PC internal speaker
 * 
 * version 1.0
 *
 * written by James Allwright 27th March 1997
 * Department of Electronics and Computer Science,
 * University of Southampton, UK
 * Comments to jra@ecs.soton.ac.uk
 * 
 * based on public domain 'midifilelib' package.
 *
 * This code may be freely distributed.
 *
 * Modified by ToughDev (www.toughdev.com)
 */

#include <stdio.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>

/* definitions for MIDI file parsing code */
int (*Mf_getc)();
int (*Mf_header)();
int (*Mf_trackstart)();
int (*Mf_trackend)();
int (*Mf_noteon)();
int (*Mf_noteoff)();
int (*Mf_pressure)();
int (*Mf_parameter)();
int (*Mf_pitchbend)();
int (*Mf_program)();
int (*Mf_chanpressure)();
int (*Mf_sysex)();
int (*Mf_metamisc)();
int (*Mf_seqspecific)();
int (*Mf_seqnum)();
int (*Mf_text)();
int (*Mf_eot)();
int (*Mf_timesig)();
int (*Mf_smpte)();
int (*Mf_tempo)();
int (*Mf_keysig)();
int (*Mf_arbitrary)();
int (*Mf_error)();
long Mf_currtime;
int Mf_nomerge;

/* definitions for MIDI file writing code */
int (*Mf_putc)();
int (*Mf_writetrack)();
int (*Mf_writetempotrack)();
float mf_ticks2sec();
long mf_sec2ticks();
void mfwrite();

int Mf_nomerge = 0;    /* 1 => continue'ed system exclusives are */
                    /* not collapsed. */
long Mf_currtime = 0L;    /* current time in delta-time units */

/* private stuff */
static long Mf_toberead = 0L;
static long Mf_numbyteswritten = 0L;

static long readvarinum();
static long read32bit();
static long to32bit();
static int read16bit();
static int to16bit();
static char *msg();


/* MIDI status commands most significant bit is 1 */
#define note_off         	0x80
#define note_on          	0x90
#define poly_aftertouch  	0xa0
#define control_change    	0xb0
#define program_chng     	0xc0
#define channel_aftertouch      0xd0
#define pitch_wheel      	0xe0
#define system_exclusive      	0xf0
#define delay_packet	 	(1111)

/* 7 bit controllers */
#define damper_pedal            0x40
#define portamento	        0x41 	
#define sostenuto	        0x42
#define soft_pedal	        0x43
#define general_4               0x44
#define	hold_2		        0x45
#define	general_5	        0x50
#define	general_6	        0x51
#define general_7	        0x52
#define general_8	        0x53
#define tremolo_depth	        0x5c
#define chorus_depth	        0x5d
#define	detune		        0x5e
#define phaser_depth	        0x5f

/* parameter values */
#define data_inc	        0x60
#define data_dec	        0x61

/* parameter selection */
#define non_reg_lsb	        0x62
#define non_reg_msb	        0x63
#define reg_lsb		        0x64
#define reg_msb		        0x65

/* Standard MIDI Files meta event definitions */
#define	meta_event		0xFF
#define	sequence_number 	0x00
#define	text_event		0x01
#define copyright_notice 	0x02
#define sequence_name    	0x03
#define instrument_name 	0x04
#define lyric	        	0x05
#define marker			0x06
#define	cue_point		0x07
#define channel_prefix		0x20
#define	end_of_track		0x2f
#define	set_tempo		0x51
#define	smpte_offset		0x54
#define	time_signature		0x58
#define	key_signature		0x59
#define	sequencer_specific	0x74

/* Manufacturer's ID number */
#define Seq_Circuits (0x01) /* Sequential Circuits Inc. */
#define Big_Briar    (0x02) /* Big Briar Inc.           */
#define Octave       (0x03) /* Octave/Plateau           */
#define Moog         (0x04) /* Moog Music               */
#define Passport     (0x05) /* Passport Designs         */
#define Lexicon      (0x06) /* Lexicon 			*/
#define Tempi        (0x20) /* Bon Tempi                */
#define Siel         (0x21) /* S.I.E.L.                 */
#define Kawai        (0x41) 
#define Roland       (0x42)
#define Korg         (0x42)
#define Yamaha       (0x43)

/* miscellaneous definitions */
#define MThd 0x4d546864
#define MTrk 0x4d54726b
#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))


/*
 * Write multi-length bytes to MIDI format files
 */
void 
WriteVarLen(value)
long value;
{
  long buffer;

  buffer = value & 0x7f;
  while((value >>= 7) > 0)
  {
  buffer <<= 8;
  buffer |= 0x80;
  buffer += (value & 0x7f);
  }
  while(1){
       eputc((char)(buffer & 0xff));
       
  if(buffer & 0x80)
    buffer >>= 8;
  else
    return;
  }
}/* end of WriteVarLen */


mfread()     /* The only non-static function in this file. */
{
  // if ( Mf_getc == NULLFUNC )
  // mferror("mfread() called without setting Mf_getc"); 

  readheader();
  while ( readtrack() )
    ;
}

/* for backward compatibility with the original lib */
midifile()
{
    mfread();
}

static
readmt(s)    /* read through the "MThd" or "MTrk" header string */
char *s;
{
  int n = 0;
  char *p = s;
  int c;

  while ( n++<4 && (c=(*Mf_getc)()) != EOF ) {
    if ( c != *p++ ) {
      char buff[32];
      (void) strcpy(buff,"expecting ");
      (void) strcat(buff,s);
      mferror(buff);
    }
  }
  return(c);
}

static
egetc()      /* read a single character and abort on EOF */
{
  int c = (*Mf_getc)();

  if ( c == EOF )
    mferror("premature EOF");
  Mf_toberead--;
  return(c);
}

static
readheader()    /* read a header chunk */
{
  int format, ntrks, division;

  if ( readmt("MThd") == EOF )
    return;

  Mf_toberead = read32bit();
  format = read16bit();
  ntrks = read16bit();
  division = read16bit();

  if ( Mf_header )
    (*Mf_header)(format,ntrks,division);

  /* flush any extra stuff, in case the length of header is not 6 */
  while ( Mf_toberead > 0 )
    (void) egetc();
}

static
readtrack()     /* read a track chunk */
{
  /* This array is indexed by the high half of a status byte.  It's */
  /* value is either the number of bytes needed (1 or 2) for a channel */
  /* message, or 0 (meaning it's not  a channel message). */
  static int chantype[] = {
    0, 0, 0, 0, 0, 0, 0, 0,    /* 0x00 through 0x70 */
    2, 2, 2, 2, 1, 1, 2, 0    /* 0x80 through 0xf0 */
  };
  long lookfor;
  int c, c1, type;
  int sysexcontinue = 0;  /* 1 if last message was an unfinished sysex */
  int running = 0;  /* 1 when running status used */
  int status = 0;    /* status value (e.g. 0x90==note-on) */
  int needed;

  if ( readmt("MTrk") == EOF )
    return(0);

  Mf_toberead = read32bit();
  Mf_currtime = 0;

  if ( Mf_trackstart )
    (*Mf_trackstart)();

  while ( Mf_toberead > 0 ) {

    Mf_currtime += readvarinum();  /* delta time */

    c = egetc();

    if ( sysexcontinue && c != 0xf7 )
      mferror("didn't find expected continuation of a sysex");

    if ( (c & 0x80) == 0 ) {   /* running status? */
      if ( status == 0 )
        mferror("unexpected running status");
      running = 1;
    }
    else {
      status = c;
      running = 0;
    }

    needed = chantype[ (status>>4) & 0xf ];

    if ( needed ) {    /* ie. is it a channel message? */

      if ( running )
        c1 = c;
      else
        c1 = egetc();
      chanmessage( status, c1, (needed>1) ? egetc() : 0 );
      continue;;
    }

    switch ( c ) {

    case 0xff:      /* meta event */

      type = egetc();
      lookfor = Mf_toberead - readvarinum();
      msginit();

      while ( Mf_toberead > lookfor )
        msgadd(egetc());

      metaevent(type);
      break;

    case 0xf0:    /* start of system exclusive */

      lookfor = Mf_toberead - readvarinum();
      msginit();
      msgadd(0xf0);

      while ( Mf_toberead > lookfor )
        msgadd(c=egetc());

      if ( c==0xf7 || Mf_nomerge==0 )
        sysex();
      else
        sysexcontinue = 1;  /* merge into next msg */
      break;

    case 0xf7:  /* sysex continuation or arbitrary stuff */

      lookfor = Mf_toberead - readvarinum();

      if ( ! sysexcontinue )
        msginit();

      while ( Mf_toberead > lookfor )
        msgadd(c=egetc());

      if ( ! sysexcontinue ) {
        if ( Mf_arbitrary )
          (*Mf_arbitrary)(msgleng(),msg());
      }
      else if ( c == 0xf7 ) {
        sysex();
        sysexcontinue = 0;
      }
      break;
    default:
      badbyte(c);
      break;
    }
  }
  if ( Mf_trackend )
    (*Mf_trackend)();
  return(1);
}

static
badbyte(c)
int c;
{
  char buff[32];

  (void) sprintf(buff,"unexpected byte: 0x%02x",c);
  mferror(buff);
}

static
metaevent(type)
{
  int leng = msgleng();
  char *m = msg();

  switch  ( type ) {
  case 0x00:
    if ( Mf_seqnum )
      (*Mf_seqnum)(to16bit(m[0],m[1]));
    break;
  case 0x01:  /* Text event */
  case 0x02:  /* Copyright notice */
  case 0x03:  /* Sequence/Track name */
  case 0x04:  /* Instrument name */
  case 0x05:  /* Lyric */
  case 0x06:  /* Marker */
  case 0x07:  /* Cue point */
  case 0x08:
  case 0x09:
  case 0x0a:
  case 0x0b:
  case 0x0c:
  case 0x0d:
  case 0x0e:
  case 0x0f:
    /* These are all text events */
    if ( Mf_text )
      (*Mf_text)(type,leng,m);
    break;
  case 0x2f:  /* End of Track */
    if ( Mf_eot )
      (*Mf_eot)();
    break;
  case 0x51:  /* Set tempo */
    if ( Mf_tempo )
      (*Mf_tempo)(to32bit(0,m[0],m[1],m[2]));
    break;
  case 0x54:
    if ( Mf_smpte )
      (*Mf_smpte)(m[0],m[1],m[2],m[3],m[4]);
    break;
  case 0x58:
    if ( Mf_timesig )
      (*Mf_timesig)(m[0],m[1],m[2],m[3]);
    break;
  case 0x59:
    if ( Mf_keysig )
      (*Mf_keysig)(m[0],m[1]);
    break;
  case 0x7f:
    if ( Mf_seqspecific )
      (*Mf_seqspecific)(leng,m);
    break;
  default:
    if ( Mf_metamisc )
      (*Mf_metamisc)(type,leng,m);
  }
}

static
sysex()
{
  if ( Mf_sysex )
    (*Mf_sysex)(msgleng(),msg());
}

static
chanmessage(status,c1,c2)
int status;
int c1, c2;
{
  int chan = status & 0xf;

  switch ( status & 0xf0 ) {
  case 0x80:
    if ( Mf_noteoff )
      (*Mf_noteoff)(chan,c1,c2);
    break;
  case 0x90:
    if ( Mf_noteon )
      (*Mf_noteon)(chan,c1,c2);
    break;
  case 0xa0:
    if ( Mf_pressure )
      (*Mf_pressure)(chan,c1,c2);
    break;
  case 0xb0:
    if ( Mf_parameter )
      (*Mf_parameter)(chan,c1,c2);
    break;
  case 0xe0:
    if ( Mf_pitchbend )
      (*Mf_pitchbend)(chan,c1,c2);
    break;
  case 0xc0:
    if ( Mf_program )
      (*Mf_program)(chan,c1);
    break;
  case 0xd0:
    if ( Mf_chanpressure )
      (*Mf_chanpressure)(chan,c1);
    break;
  }
}

/* readvarinum - read a varying-length number, and return the */
/* number of characters it took. */

static long
readvarinum()
{
  long value;
  int c;

  c = egetc();
  value = c;
  if ( c & 0x80 ) {
    value &= 0x7f;
    do {
      c = egetc();
      value = (value << 7) + (c & 0x7f);
    } while (c & 0x80);
  }
  return (value);
}

static long
to32bit(c1,c2,c3,c4)
{
  long value = 0L;

  value = (c1 & 0xff);
  value = (value<<8) + (c2 & 0xff);
  value = (value<<8) + (c3 & 0xff);
  value = (value<<8) + (c4 & 0xff);
  return (value);
}

static
to16bit(c1,c2)
int c1, c2;
{
  return ((c1 & 0xff ) << 8) + (c2 & 0xff);
}

static long
read32bit()
{
  int c1, c2, c3, c4;

  c1 = egetc();
  c2 = egetc();
  c3 = egetc();
  c4 = egetc();
  return to32bit(c1,c2,c3,c4);
}

static
read16bit()
{
  int c1, c2;
  c1 = egetc();
  c2 = egetc();
  return to16bit(c1,c2);
}

/* static */
mferror(s)
char *s;
{
  if ( Mf_error )
    (*Mf_error)(s);
  exit(1);
}

/* The code below allows collection of a system exclusive message of */
/* arbitrary length.  The Msgbuff is expanded as necessary.  The only */
/* visible data/routines are msginit(), msgadd(), msg(), msgleng(). */

#define MSGINCREMENT 128
static char *Msgbuff = NULL;  /* message buffer */
static int Msgsize = 0;    /* Size of currently allocated Msg */
static int Msgindex = 0;  /* index of next available location in Msg */

static
msginit()
{
  Msgindex = 0;
}

static char *
msg()
{
  return(Msgbuff);
}

static
msgleng()
{
  return(Msgindex);
}

static
msgadd(c)
int c;
{
  /* If necessary, allocate larger message buffer. */
  if ( Msgindex >= Msgsize )
    biggermsg();
  Msgbuff[Msgindex++] = c;
}

static
biggermsg()
{
/*   char *malloc(); */
  char *newmess;
  char *oldmess = Msgbuff;
  int oldleng = Msgsize;

  Msgsize += MSGINCREMENT;
  newmess = (char *) malloc( (unsigned)(sizeof(char)*Msgsize) );

  if(newmess == NULL)
    mferror("malloc error!");
    
  /* copy old message into larger new one */
  if ( oldmess != NULL ) {
    register char *p = newmess;
    register char *q = oldmess;
    register char *endq = &oldmess[oldleng];

    for ( ; q!=endq ; p++,q++ )
      *p = *q;
    free(oldmess);
  }
  Msgbuff = newmess;
}

/*
 * mfwrite() - The only fuction you'll need to call to write out
 *             a midi file.
 *
 * format      0 - Single multi-channel track
 *             1 - Multiple simultaneous tracks
 *             2 - One or more sequentially independent
 *                 single track patterns                
 * ntracks     The number of tracks in the file.
 * division    This is kind of tricky, it can represent two
 *             things, depending on whether it is positive or negative
 *             (bit 15 set or not).  If  bit  15  of division  is zero,
 *             bits 14 through 0 represent the number of delta-time
 *             "ticks" which make up a quarter note.  If bit  15 of
 *             division  is  a one, delta-times in a file correspond to
 *             subdivisions of a second similiar to  SMPTE  and  MIDI
 *             time code.  In  this format bits 14 through 8 contain
 *             one of four values - 24, -25, -29, or -30,
 *             corresponding  to  the  four standard  SMPTE and MIDI
 *             time code frame per second formats, where  -29
 *             represents  30  drop  frame.   The  second  byte
 *             consisting  of  bits 7 through 0 corresponds the the
 *             resolution within a frame.  Refer the Standard MIDI
 *             Files 1.0 spec for more details.
 * fp          This should be the open file pointer to the file you
 *             want to write.  It will have be a global in order
 *             to work with Mf_putc.  
 */ 
void 
mfwrite(format,ntracks,division,fp) 
int format,ntracks,division; 
FILE *fp; 
{
    int i; void mf_write_track_chunk(), mf_write_header_chunk();

    // if ( Mf_putc == NULLFUNC )
    //  mferror("mfmf_write() called without setting Mf_putc");

    // if ( Mf_writetrack == NULLFUNC )
    //  mferror("mfmf_write() called without setting Mf_mf_writetrack"); 

    /* every MIDI file starts with a header */
    mf_write_header_chunk(format,ntracks,division);

    /* In format 1 files, the first track is a tempo map */
    if(format == 1 && ( Mf_writetempotrack ))
    {
  (*Mf_writetempotrack)();
    }

    /* The rest of the file is a series of tracks */
    for(i = 0; i < ntracks; i++)
        mf_write_track_chunk(i,fp);
}

void 
mf_write_track_chunk(which_track,fp)
int which_track;
FILE *fp;
{
  long trkhdr,trklength;
  long offset, place_marker;
  void write16bit(),write32bit();
  // void WriteVarLen();
  long endspace;
  
  
  trkhdr = MTrk;
  trklength = 0;

  /* Remember where the length was written, because we don't
     know how long it will be until we've finished writing */
  offset = lseek(fp, (long) 0, 1); 
  printf("offset = %d\n", offset);

#ifdef DEBUG
        printf("offset = %d\n",(int) offset);
#endif

  /* Write the track chunk header */
/*
  write32bit(trkhdr);
  write32bit(trklength);
*/

  Mf_numbyteswritten = 0L; /* the header's length doesn't count */

  // setbuffering(1);
	if( Mf_writetrack )
	{
	    (*Mf_writetrack)(which_track);
	}
	/* mf_write End of track meta event */
	eputc(0);
	eputc(meta_event);
	eputc(end_of_track);

 	eputc(0);

	trklength = Mf_numbyteswritten;
  // setbuffering(0);

	/* Re-mf_write the track chunk header with right length */
	write32bit(trkhdr);
	write32bit(trklength);
  // flushbuffer();
	 
  /* It's impossible to know how long the track chunk will be beforehand,
           so the position of the track length data is kept so that it can
           be written after the chunk has been generated */
/*
  place_marker = lseek(fp, (long) 0, 1);
  printf("place_marker = %d\n", place_marker);
*/
  
  /* This method turned out not to be portable because the
           parameter returned from ftell is not guaranteed to be
           in bytes on every machine */
   /* track.length = place_marker - offset - (long) sizeof(track); */

#ifdef DEBUG
printf("length = %d\n",(int) trklength);
#endif

} /* End gen_track_chunk() */


void 
mf_write_header_chunk(format,ntracks,division)
int format,ntracks,division;
{
    long ident,length;
    void write16bit(),write32bit();
    
    ident = MThd;           /* Head chunk identifier                    */
    length = 6;             /* Chunk length                             */

    /* individual bytes of the header must be written separately
       to preserve byte order across cpu types :-( */
    write32bit(ident);
    write32bit(length);
    write16bit(format);
    write16bit(ntracks);
    write16bit(division);
} /* end gen_header_chunk() */


/*
 * mf_write_midi_event()
 * 
 * Library routine to mf_write a single MIDI track event in the standard MIDI
 * file format. The format is:
 *
 *                    <delta-time><event>
 *
 * In this case, event can be any multi-byte midi message, such as
 * "note on", "note off", etc.      
 *
 * delta_time - the time in ticks since the last event.
 * type - the type of meta event.
 * chan - The midi channel.
 * data - A pointer to a block of chars containing the META EVENT,
 *        data.
 * size - The length of the meta-event data.
 */
int 
mf_write_midi_event(delta_time, type, chan, data, size)
long delta_time;
int chan,type;
int size;
char *data;
{
    int i;
    // void WriteVarLen();
    char c;

    WriteVarLen(delta_time);

    /* all MIDI events start with the type in the first four bits,
       and the channel in the lower four bits */
    c = (char) (type | chan);

    if(chan > 15)
        mferror("error: MIDI channel greater than 16\n");

    eputc(c);

    /* write out the data bytes */
    for(i = 0; i < size; i++)
  eputc(data[i]);

    return(size);
} /* end mf_write MIDI event */

/*
 * mf_write_meta_event()
 *
 * Library routine to mf_write a single meta event in the standard MIDI
 * file format. The format of a meta event is:
 *
 *          <delta-time><FF><type><length><bytes>
 *
 * delta_time - the time in ticks since the last event.
 * type - the type of meta event.
 * data - A pointer to a block of chars containing the META EVENT,
 *        data.
 * size - The length of the meta-event data.
 */
int
mf_write_meta_event(delta_time, type, data, size)
long delta_time;
char *data;
int type;
int size;
{
    int i;

    WriteVarLen(delta_time);
    
    /* This marks the fact we're writing a meta-event */
    eputc(meta_event);

    /* The type of meta event */
    eputc(type);

    /* The length of the data bytes to follow */
    WriteVarLen((long)size); 

    for(i = 0; i < size; i++)
    {
  if(eputc((data[i] & 0xff)) != (data[i] & 0xff))
      return(-1); 
    }
    return(size);
} /* end mf_write_meta_event */

void 
mf_write_tempo(tempo)
long tempo;
{
    /* Write tempo */
    /* all tempos are written as 120 beats/minute, */
    /* expressed in microseconds/quarter note     */
    eputc(0);
    eputc(meta_event);
    eputc(set_tempo);

    eputc(3);
    eputc((char)(0xff & (tempo >> 16)));
    eputc((char)(0xff & (tempo >> 8)));
    eputc((char)(0xff & tempo));
}

long 
mf_sec2ticks(secs,division,tempo)
int division;
long tempo;
float secs;
{    
     return (long)(((secs * 1000.0) / 4.0 * division) / tempo);
}

/* 
 * This routine converts delta times in ticks into seconds. The
 * else statement is needed because the formula is different for tracks
 * based on notes and tracks based on SMPTE times.
 *
 */
float 
mf_ticks2sec(ticks,division,tempo)
int division;
long tempo;
long ticks;
{
    float ret;
    float smpte_format, smpte_resolution;

    if(division > 0)
        ret = ((float) (((float)(ticks) * (float)(tempo)) / ((float)(division) * 1000000.0)));
    else
    {
       smpte_format = upperbyte(division);
       smpte_resolution = lowerbyte(division);
       ret = (float) ((float) ticks / (smpte_format * smpte_resolution * 1000000.0));
    }
    return (ret);
} /* end of ticks2sec() */


/*
 * write32bit()
 * write16bit()
 *
 * These routines are used to make sure that the byte order of
 * the various data types remains constant between machines. This
 * helps make sure that the code will be portable from one system
 * to the next.  It is slightly dangerous that it assumes that longs
 * have at least 32 bits and ints have at least 16 bits, but this
 * has been true at least on PCs, UNIX machines, and Macintosh's.
 *
 */
void 
write32bit(data)
long data;
{
    eputc((char)((data >> 24) & 0xff));
    eputc((char)((data >> 16) & 0xff));
    eputc((char)((data >> 8 ) & 0xff));
    eputc((char)(data & 0xff));
}

void 
write16bit(data)
int data;
{
    eputc((char)((data & 0xff00) >> 8));
    eputc((char)(data & 0xff));
}

/* write a single character and abort on error */
int eputc(c)      
char c;
{
  int return_val;
  
  /*
  if((Mf_putc) == NULLFUNC)
  {
    mferror("Mf_putc undefined");
    return(-1);
  }
  */
  
  return_val = (Mf_putc)(c);

  if ( return_val == EOF )
    mferror("error writing");
    
  Mf_numbyteswritten++;
  return(return_val);
}

void mfwrite();

/* MIDI status commands most significant bit is 1 */
#define note_off         	0x80
#define note_on          	0x90
#define poly_aftertouch  	0xa0
#define control_change    	0xb0
#define program_chng     	0xc0
#define channel_aftertouch      0xd0
#define pitch_wheel      	0xe0
#define system_exclusive      	0xf0
#define delay_packet	 	(1111)

/* 7 bit controllers */
#define damper_pedal            0x40
#define portamento	        0x41 	
#define sostenuto	        0x42
#define soft_pedal	        0x43
#define general_4               0x44
#define	hold_2		        0x45
#define	general_5	        0x50
#define	general_6	        0x51
#define general_7	        0x52
#define general_8	        0x53
#define tremolo_depth	        0x5c
#define chorus_depth	        0x5d
#define	detune		        0x5e
#define phaser_depth	        0x5f

/* parameter values */
#define data_inc	        0x60
#define data_dec	        0x61

/* parameter selection */
#define non_reg_lsb	        0x62
#define non_reg_msb	        0x63
#define reg_lsb		        0x64
#define reg_msb		        0x65

/* Standard MIDI Files meta event definitions */
#define	meta_event		0xFF
#define	sequence_number 	0x00
#define	text_event		0x01
#define copyright_notice 	0x02
#define sequence_name    	0x03
#define instrument_name 	0x04
#define lyric	        	0x05
#define marker			0x06
#define	cue_point		0x07
#define channel_prefix		0x20
#define	end_of_track		0x2f
#define	set_tempo		0x51
#define	smpte_offset		0x54
#define	time_signature		0x58
#define	key_signature		0x59
#define	sequencer_specific	0x74

/* Manufacturer's ID number */
#define Seq_Circuits (0x01) /* Sequential Circuits Inc. */
#define Big_Briar    (0x02) /* Big Briar Inc.           */
#define Octave       (0x03) /* Octave/Plateau           */
#define Moog         (0x04) /* Moog Music               */
#define Passport     (0x05) /* Passport Designs         */
#define Lexicon      (0x06) /* Lexicon 			*/
#define Tempi        (0x20) /* Bon Tempi                */
#define Siel         (0x21) /* S.I.E.L.                 */
#define Kawai        (0x41) 
#define Roland       (0x42)
#define Korg         (0x42)
#define Yamaha       (0x43)

/* miscellaneous definitions */
#define MThd 0x4d546864
#define MTrk 0x4d54726b
#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))


static FILE *F;
int SECONDS;    /* global that tells whether to display seconds or ticks */
int division;    /* from the file header */
long tempo = 500000; /* the default tempo is 120 beats/minute */
int unitlen;
int channel;
int track, trackno, settrack;
int format;
int active[256];
int transpose;
int tempofactor;

struct note_type {
  long start, stop;
  int pitch;
  struct note_type *next;
};

struct note_type *head;
struct note_type *tail;

struct note_type* create(my_pitch, now)
int my_pitch;
long now;
{
  struct note_type* newitem;

  newitem = (struct note_type*) malloc(sizeof(struct note_type));
  if (newitem == NULL) {
    printf("Could not allocate memory for note\n");
    exit(0);
  };
  newitem->next = NULL;
  newitem->start = now;
  newitem->pitch = my_pitch;
  return(newitem);
};

change(newpitch, time)
int newpitch;
long time;
{
  if (time == tail->start) {
    tail->pitch = newpitch;
  } else {
    tail->stop = time;
    tail->next = create(newpitch, time);
    tail = tail->next;
  };
};

filegetc()
{
    return(getc(F));
}

/* for crack */
extern int arg_index;

int readnum(num) 
char *num;
{
  int t;
  char *p;
  
  t = 0;
  p = num;
  while (((int)*p >= '0') && ((int)*p <= '9')) {
    t = t * 10 + (int) *p - '0';
    p = p + 1;
  };
  return t;
};

int readnump(p) 
char **p;
{
  int t;
  
  t = 0;
  while (((int)**p >= '0') && ((int)**p <= '9')) {
    t = t * 10 + (int) **p - '0';
    *p = *p + 1;
  };
  return t;
};

int getarg(option, argc, argv)
char *option;
char *argv[];
int argc;
{
  int j, place;

  place = -1;
  for (j=0; j<argc; j++) {
    if (strcmp(option, argv[j]) == 0) {
      place = j + 1;
    };
  };
  return (place);
};

main(argc,argv)
char *argv[];
int argc;
{
  FILE *efopen();
  // void note();
  // void clearkey();
  float mf_ticks2sec();
  int j;
  int arg;
  int i, interval[256];
  double conv, freq;
  struct note_type *place;

  /* set up array of delay times corresponding to notes */
  conv = log((double) 2);
  for (i=0; i<256; i++) {
    freq = (double) exp((double) (conv * (i-69))/12.0) * 440;
    interval[i] = (int) (((double) 1193180)/freq + (double) 0.5);
  };

  /* initialize linked list */
  head = create(-1, (long) 0);
  tail = head;
  for (i=0; i<256; i++) {
    active[i] = 0;
  };

  arg = getarg("-i", argc, argv);
  if (arg != -1) {
    printf("\n     Midiplay\n");
    printf("     --------\n");
    printf("written March 1997 by James Allwright. Modified by ToughDev (www.toughdev.com) in 2018.\n");
    printf("using the midifilelib MIDI library.\n\n");
    printf("This program plays a MIDI file through the PC's internal\n");
    printf("speaker. The output is monophonic, so some notes may be lost.\n");
    printf("Where two or more notes should be played simultaneously,\n");
    printf("midiplay plays the highest pitch one. If the MIDI file is\n");
    printf("type 1, midiplay chooses the first non-empty track to play\n");
    printf("unless some other track has been specified with the -t option.\n");
    printf("While a file is playing, press any key to stop it.\n");
    exit(0);
  };
  arg = 1;
  if (arg < argc) {
    F = efopen(argv[arg],"rb");
  } else {
    printf("midiplay version 1.0\n  usage :\n");
    printf("midiplay midifile <options>\n");
    printf("         -t <track>\n");
    printf("         -c <channel>\n");
    printf("         -s <semitones to transpose>\n");
    printf("         -p <percentage of original speed> 100 = no change\n");
    printf("         -i more information\n");
    exit(0);
  };
  arg = getarg("-t", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    track = readnum(argv[arg]);
    settrack = 1;
  } else {
    track = 0;
    settrack = 0;
  };
  arg = getarg("-p", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    tempofactor = readnum(argv[arg]) - 1;
  } else {
    tempofactor = 100;
  };
  arg = getarg("-c", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    channel = readnum(argv[arg]) - 1;
  } else {
    channel = -1;
  };
  transpose = 0;
  arg = getarg("-s", argc, argv);
  if ((arg != -1) && (arg < argc)) {
    int t;

    if (*argv[arg] == '-') {
      transpose = - readnum(argv[arg] + 1);
    } else {
      transpose = readnum(argv[arg]);
    };
  };
  trackno = 0;
  initfuncs();
  Mf_getc = filegetc;
  midifile();
  fclose(F);
  /* finish off tune */
  tail->stop = Mf_currtime;
  /* post-process tune */
  place = head;
  while (place != NULL) {
    long dt;

    if (place->stop > place->start) {
      dt = place->stop - place->start;
      place->start = (long) (1000000.0 * mf_ticks2sec(dt, division, tempo)
                                       * ((float) tempofactor/100.0));
    } else {
     place->start = (long) 0;
    };
    if (place->pitch != -1) {
      place->pitch = interval[place->pitch];
    };
    place = place->next;
  };
  /* play tune */
  place = head;
  while (place != NULL) {
    // note(place->pitch, place->start);
	if (place->pitch == -1)
	{
		nosound();
	}
	else {
		// printf("pitch=%u start=%u\r\n", place->pitch, place->start);
		
		sound(1193180 / place->pitch);
		delay(place->start / 1000);
	}
    place = place->next;
	if (kbhit()) 
		break;
  };
  nosound();
  // clearkey();
  /* free up space */
  place = head;
  while (place != NULL) {
    tail = place->next;
    free(place);
    place = tail;
  };
  // printf("Done. Exiting ...");
};

FILE *
efopen(name,mode)
char *name;
char *mode;
{
    FILE *f;
/*
    extern int errno;
    extern char *sys_errlist[];
    extern int sys_nerr;
    char *errmess;
*/

    if ( (f=fopen(name,mode)) == NULL ) {
        (void) fprintf(stderr,"*** ERROR *** Cannot open '%s'!\n",name);
/*
        if ( errno <= sys_nerr )
            errmess = sys_errlist[errno];
        else
            errmess = "Unknown error!";
        (void) fprintf(stderr,"************* Reason: %s\n",errmess);
*/
        exit(1);
    }
    return(f);
}

error(s)
char *s;
{
    fprintf(stderr,"Error: %s\n",s);
}

txt_header(xformat,ntrks,ldivision)
{
  division = ldivision; 
  format = xformat;
}

txt_trackstart()
{
  trackno = trackno + 1;
  /* read from first non-empty track */
  if ((settrack == 0) && (head == tail)) {
    track = trackno;
  };
}

txt_trackend()
{
}

int trackfilter()
{
   return ((track == 0) || (track == trackno));
};

int chantrackfilter(chan)
int chan;
{
  return (((channel == -1) || (channel == chan)) &&
          ((track == 0) || (track == trackno)));
};

do_on(pitch)
int pitch;
{
  active[pitch] = 1;
  if (pitch > tail->pitch) {
    change(pitch, Mf_currtime);
  };
};

do_off(pitch)
int pitch;
{
  int i, nextpitch;

  active[pitch] = 0;
  nextpitch = -1;
  i = 255;
  while ((i>-1) && (nextpitch == -1)) {
    if (active[i] == 1) {
      nextpitch = i;
    };
    i = i - 1;
  };
  change(nextpitch, Mf_currtime);
};

int clip(x)
int x;
{
  int y;

  y = x;
  if (y > 255) y = 255; 
  if (y < 0)   y = 0;
  return(y);
};

txt_noteon(chan,pitch,vol)
{
  if (chantrackfilter(chan)) {
    if (vol != 0) {
      do_on(clip(pitch+transpose));
    } else {
      do_off(clip(pitch+transpose));
    };
  };
}

txt_noteoff(chan,pitch,vol)
{
  if (chantrackfilter(chan)) {
    do_off(clip(pitch+transpose));
  };
}

txt_pressure(chan,pitch,press)
{
}

txt_parameter(chan,control,value)
{
}

txt_pitchbend(chan,msb,lsb)
{
}

txt_program(chan,program)
{
}

txt_chanpressure(chan,press)
{
}

txt_sysex(leng,mess)
char *mess;
{
}

txt_metamisc(type,leng,mess)
char *mess;
{
}

txt_metaspecial(type,leng,mess)
char *mess;
{
}

txt_metatext(type,leng,mess)
char *mess;
{ 
}

txt_metaseq(num)
{  
}

txt_metaeot()
{
}

txt_keysig(sf,mi)
char sf, mi;
{
}

txt_tempo(ltempo)
long ltempo;
{
    tempo = ltempo;
}

txt_timesig(nn,dd,cc,bb)
{
}

txt_smpte(hr,mn,se,fr,ff)
{
}

txt_arbitrary(leng,mess)
char *mess;
{
}

initfuncs()
{
    Mf_error = error;
    Mf_header =  txt_header;
    Mf_trackstart =  txt_trackstart;
    Mf_trackend =  txt_trackend;
    Mf_noteon =  txt_noteon;
    Mf_noteoff =  txt_noteoff;
    Mf_pressure =  txt_pressure;
    Mf_parameter =  txt_parameter;
    Mf_pitchbend =  txt_pitchbend;
    Mf_program =  txt_program;
    Mf_chanpressure =  txt_chanpressure;
    Mf_sysex =  txt_sysex;
    Mf_metamisc =  txt_metamisc;
    Mf_seqnum =  txt_metaseq;
    Mf_eot =  txt_metaeot;
    Mf_timesig =  txt_timesig;
    Mf_smpte =  txt_smpte;
    Mf_tempo =  txt_tempo;
    Mf_keysig =  txt_keysig;
    Mf_seqspecific =  txt_metaspecial;
    Mf_text =  txt_metatext;
    Mf_arbitrary =  txt_arbitrary;
}
