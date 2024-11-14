use std::sync::{Arc, RwLock};
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use crossbeam_channel::Receiver;
use std::time::Duration;
use symphonia::core::audio::{AudioBufferRef};
use symphonia::core::codecs::{DecoderOptions, CODEC_TYPE_NULL};
use symphonia::core::formats::FormatOptions;
use symphonia::core::io::MediaSourceStream;
use symphonia::core::meta::MetadataOptions;
use symphonia::core::probe::Hint;
use crate::shared::{AudioMessage, SharedState};

pub(crate) fn run_audio_thread(
    shared_state: Arc<RwLock<SharedState>>,
    audio_rx: Receiver<AudioMessage>,
) -> Result<(), Box<dyn std::error::Error>> {
    // Open the media source.
    let src = std::fs::File::open("./resources/gec.mp3").expect("failed to open media");

    // Create the media source stream.
    let mss = MediaSourceStream::new(Box::new(src), Default::default());

    let mut hint = Hint::new();
    hint.with_extension("mp3");

    // Use the default options for metadata and format readers.
    let meta_opts: MetadataOptions = Default::default();
    let fmt_opts: FormatOptions = Default::default();

    // Probe the media source.
    let probed = symphonia::default::get_probe().format(&hint, mss, &fmt_opts, &meta_opts)
        .expect("unsupported format");

    // Get the instantiated format reader.
    let mut format = probed.format;

    let track = format.tracks()
        .iter()
        .find(|t| t.codec_params.codec != CODEC_TYPE_NULL)
        .expect("no supported audio tracks");

    // Use the default options for the decoder.
    let dec_opts: DecoderOptions = Default::default();

    // Create a decoder for the track.
    let mut decoder = symphonia::default::get_codecs().make(&track.codec_params, &dec_opts)
        .expect("unsupported codec");

    let host = cpal::default_host();
    let device = host.default_output_device()
        .ok_or("No output device available")?;

    let config = device.default_output_config()?;
    let sample_rate = config.sample_rate().0 as f32;

    let mut iter: i32 = 0;
    let mut vecf = Vec::<f32>::new();

    loop {
        let packet = format.next_packet();

        match packet {
            Ok(packet) => {
                match decoder.decode(&packet).unwrap() {
                    AudioBufferRef::F32(buf) => {
                        for planes in buf.planes().planes() {
                            for &sample in planes.iter() {
                                vecf.push(sample);
                            }
                        }
                    }
                    _ => {
                        unimplemented!()
                    }
                }
            }
            _ => {
                break;
            }
        }
    }

    print!("Finished reading");

    let state = shared_state.clone();

    let stream = device.build_output_stream(
        &config.into(),
        move |data: &mut [f32], _: &cpal::OutputCallbackInfo| {
            let val = vecf.get(iter as usize);

            let towrite = match val {
                Some(&sample) => sample, // Get the value if available
                None => 0.0, // Default to 0.0 if out of bounds
            };

            for sample in data.iter_mut() {
                *sample = towrite;
            }
            iter += 440;

            // // Only read `shared_state` once per callback
            // let state = state.read().unwrap();
            // let (is_playing, volume, frequency) = {
            //     (state.is_playing, state.volume, state.frequency)
            // };
            //
            // if is_playing {
            //     let phase_increment = 2.0 * PI * frequency / sample_rate;
            //
            //     // Generate sine wave for each sample
            //     for sample in data.iter_mut() {
            //         *sample = (phase.sin()) * volume;
            //         phase = (phase + phase_increment) % (2.0 * PI);
            //     }
            // } else {
            //     // Output silence when not playing
            //     for sample in data.iter_mut() {
            //         *sample = 0.0;
            //     }
            // }
        },
        move |err| eprintln!("Audio stream error: {}", err),
        Option::Some(Duration::from_secs(2)),
    )?;

    stream.play()?;

    // Handle messages from GUI thread
    while let Ok(message) = audio_rx.recv() {
        match message {
            AudioMessage::SetVolume(vol) => {
                shared_state.write().unwrap().volume = vol;
            }
            AudioMessage::SetFrequency(freq) => {
                shared_state.write().unwrap().frequency = freq;
            }
            AudioMessage::Play => {
                shared_state.write().unwrap().is_playing = true;
            }
            AudioMessage::Stop => {
                shared_state.write().unwrap().is_playing = false;
            }
        }
    }

    Ok(())
}
