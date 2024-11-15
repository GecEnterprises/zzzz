use std::sync::Arc;
use symphonia::core::codecs::{DecoderOptions, CODEC_TYPE_NULL};
use symphonia::core::formats::{FormatOptions, FormatReader, Track};
use symphonia::core::io::MediaSourceStream;
use symphonia::core::meta::MetadataOptions;
use symphonia::core::probe::Hint;
use ringbuf::{traits::*, CachingCons, CachingProd, HeapRb, SharedRb};
use ringbuf::storage::Heap;
use symphonia::core::audio::{AudioBufferRef};
use symphonia::core::errors::Error;

pub(crate) struct AudioFsPlayer {
    track: Track,
    format: Box<dyn FormatReader>,
    rb_prod_chan1: CachingProd<Arc<SharedRb<Heap<f32>>>>,
}

impl AudioFsPlayer {
    pub fn load(path: &str, prod: CachingProd<Arc<SharedRb<Heap<f32>>>>) -> Result<Self, Box<dyn std::error::Error>> {
        let src = std::fs::File::open(&path)?;

        let mss = MediaSourceStream::new(Box::new(src), Default::default());

        let mut hint = Hint::new();
        hint.with_extension("mp3");

        let meta_opts: MetadataOptions = Default::default();
        let fmt_opts: FormatOptions = Default::default();

        let probed = symphonia::default::get_probe().format(&hint, mss, &fmt_opts, &meta_opts)?;

        let format = probed.format;

        let track = format.tracks()
            .iter()
            .find(|t| t.codec_params.codec != CODEC_TYPE_NULL)
            .expect("no supported audio tracks");

        Ok(AudioFsPlayer{track: track.clone(), format, rb_prod_chan1: prod})
    }

    pub fn arm(&mut self) {
        let dec_opts: DecoderOptions = Default::default();

        let mut decoder = symphonia::default::get_codecs().make(&self.track.codec_params, &dec_opts)
            .expect("unsupported codec");

        loop {
            if self.rb_prod_chan1.is_full() {
                continue;
            }

            let packet = match self.format.next_packet() {
                Ok(packet) => packet,
                Err(Error::ResetRequired) => {
                    decoder.reset();
                    continue;
                },
                Err(e) => {
                    eprintln!("Error reading packet: {}", e);
                    break;
                }
            };

            let decoded = decoder.decode(&packet).unwrap();

            match decoded {
                AudioBufferRef::F32(buf) => {
                    let planes = buf.planes();

                    let mut which_plane = 0;
                    for plane in planes.planes() {
                        for &sample in plane.iter() {
                            match which_plane {
                                0 => {
                                    self.rb_prod_chan1.try_push(sample).expect("ringbuf push failed");
                                }
                                1 => {

                                }
                                _ => {}
                            }
                        }
                        which_plane += 1;
                    }
                }
                _ => {
                    // Repeat for the different sample formats.
                    unimplemented!()
                }
            }
        }
    }
}
