use std::f32::consts::PI;
use std::time::Duration;
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use tracing::{error, info};
use crate::audio::AudioProducer;

pub(crate) struct OutHandler {
    host: cpal::Host,
    device: cpal::Device,
    config: cpal::SupportedStreamConfig
}

impl OutHandler {
    pub fn init() -> Result<Self, Box<dyn std::error::Error>> {
        let host = cpal::default_host();

        let device = host
            .default_output_device()
            .ok_or_else(|| anyhow::Error::msg("Default output device is not available"))?;
        info!("Output device : {}", device.name()?);

        let config = device.default_output_config()?;
        info!("Default output config : {:?}", config);

        Ok(OutHandler{
            host,
            device,
            config
        })
    }

    pub fn open_stream(&self) -> anyhow::Result<()> {
        let err_fn = |err| error!("Error building output sound stream: {}", err);
        let mut phase: f32 = 0.0;
        let volume = 0.2;
        let frequency = 140.0;
        let sample_rate = self.config.sample_rate().0 as f32;

        let config = self.config.clone();

        let stream_config: cpal::StreamConfig = config.into();
        let stream_config = cpal::StreamConfig{
            buffer_size: cpal::BufferSize::Fixed(1024),
            channels: stream_config.channels,
            sample_rate: stream_config.sample_rate
        };

        info!("Stream config : {:?}", stream_config);
        let num_channels = stream_config.channels as usize;

        let str = self.device.build_output_stream(
            &stream_config,
            move |data: &mut [f32], _: &cpal::OutputCallbackInfo| {
                for frame in data.chunks_mut(num_channels) {
                    let phase_increment = 2.0 * PI * frequency / sample_rate;

                    // Generate sine wave for each sample
                    let mut chan_count = 1.0;
                    for sample in frame.iter_mut() {
                        *sample = (phase.sin()) * (volume / chan_count);
                        phase = (phase + phase_increment) % (2.0 * PI);

                        chan_count += 2.0;
                    }
                }
            },
            err_fn,
            Some(Duration::from_secs(2)),
        )?;

        str.play()?;

        loop {
        }

        Ok(())
    }

    pub fn add_producer(&self, producer: impl AudioProducer) {

    }
}
