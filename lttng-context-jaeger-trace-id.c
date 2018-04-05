/*
 * lttng-context-jaeger-trace-id.c
 *
 * LTTng Jaeger trace ID context.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng-tracer.h>

static
size_t jaeger_trace_id_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(uint64_t));
	size += sizeof(uint64_t);
	return size;
}

static
void jaeger_trace_id_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	uint64_t trace_id;

	trace_id = current->jaeger_trace_id;
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(trace_id));
	chan->ops->event_write(ctx, &trace_id, sizeof(trace_id));
}

static
void jaeger_trace_id_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = current->jaeger_trace_id;
}

int lttng_add_jaeger_trace_id_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "jaeger_trace_id")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "jaeger_trace_id";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.basic.integer.size = sizeof(uint64_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.alignment = lttng_alignof(uint64_t) * CHAR_BIT;
	field->event_field.type.u.basic.integer.signedness = lttng_is_signed_type(uint64_t);
	field->event_field.type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.basic.integer.base = 10;
	field->event_field.type.u.basic.integer.encoding = lttng_encode_none;
	field->get_size = jaeger_trace_id_get_size;
	field->record = jaeger_trace_id_record;
	field->get_value = jaeger_trace_id_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_jaeger_trace_id_to_ctx);
